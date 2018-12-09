#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"
#include "../header/helper_functions.h"
#include "../header/daindex.h"
#include "../header/results.h"

#include <iostream>

using namespace std;

tempResults *queryExecute(Query *qr, relationArray *relArray)
{
	Query *orderedQuery = queryReorder(qr);												// Reorders predicates in query for optimization purposes
	tempResults *tRes = new tempResults;

	std::vector<predicates*>::iterator it;
	for( it = orderedQuery->p.begin(); it != orderedQuery->p.end(); it++)
		if( (*it)->type == JOIN)		continue;										// Each predicate is either a join or a filter
		else							relation_filter((*it),relArray,tRes);

	delete orderedQuery;

	return NULL;
}


Query *queryReorder(Query *qr)
{
	Query *newQr = new Query;

	newQr->p = qr->p;
	newQr->relations = qr->relations;
	newQr->checksums = qr->checksums;

	return newQr;
}

void relation_filter(predicates *pred, relationArray *rArray, tempResults *tr)
{

	int relationId = pred->relation1;
	int columnId = pred->column1;
	uint64_t filter = pred->filter;

	Relations * currentRelation = rArray->relations.at(relationId);						// Fetch relation

	uint64_t *size = new uint64_t;
	uint64_t *rowids = tempResultsLookup(tr, relationId, size);							// Also check if relation exists on tempResults

	std::vector<uint64_t> results;														// Result vector

	if(rowids != NULL )																	// If relation exists on tempResults
	{

		for(uint64_t row=0; row < *size; row++)
		{
			uint64_t rid = rowids[row];

			switch (pred->type) {
				case EQ_FILTER:
					if( filter == currentRelation->relation[columnId][rid])
						results.push_back(rid);
					break;

				case GT_FILTER:
					if( filter <= currentRelation->relation[columnId][rid])
						results.push_back(rid);
					break;

				case LT_FILTER:
					if( filter >= currentRelation->relation[columnId][rid])
						results.push_back(rid);
					break;

				default:
					//This should never happen
					break;
			}
		}

		tempResultsUpdate(results,relationId,tr);
	}
	else
	{
		for(uint64_t row=0; row < currentRelation->size; row++)
		{

			switch (pred->type) {
				case EQ_FILTER:
					if( filter == currentRelation->relation[columnId][row])
						results.push_back(row);
					break;

				case GT_FILTER:
					if( filter <= currentRelation->relation[columnId][row])
						results.push_back(row);
					break;

				case LT_FILTER:
					if( filter >= currentRelation->relation[columnId][row])
						results.push_back(row);
					break;

				default:
					//This should never happen
					break;
			}
		}
		// TODO: tempResultsAdd(results,relation1,tr);
	}
}


void relation_join(predicates *pred, relationArray *rArray)
{
	int relationId1 = pred->relation1;
	int relationId2 = pred->relation2;
	int columnId1   = pred->column1;
	int columnId2   = pred->column2;

	Relations * currentRelation1 = rArray->relations.at(relationId1);
	Relations * currentRelation2 = rArray->relations.at(relationId2);
	uint64_t *rowids;
	// TODO: check temp_results

	// TODO: check fringe case if relation1 = relation2

	uint64_t *rowID1 = createRowID(currentRelation1->size);
	uint64_t *rowID2 = createRowID(currentRelation1->size);

	Table_Info *tableInfo1 = init_table_info(rowID1,currentRelation1->relation[columnId1],currentRelation1->size);
	Table_Info *tableInfo2 = init_table_info(rowID2,currentRelation2->relation[columnId2],currentRelation2->size);

	daIndex **indx;
	result *res;

	if(rowID1 < rowID2)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx);
	}
	else
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx);
	}

	// TODO: update temp_results

	return /*res*/;
}


uint64_t *createRowID(uint64_t rSize)
{
	uint64_t *rowID = new uint64_t[rSize];
	for(uint64_t i=0; i < rSize; i++)	rowID[i] = i;
	return rowID;
}


uint64_t *tempResultsLookup(tempResults *tpr, int relationId, uint64_t *size)
{
	if( tpr == NULL )	return NULL;

	if( tpr->res.size() == 0 )	return NULL;

	std::vector<tempResultArray>::iterator it1;

	for(it1 = tpr->res.begin(); it1 != tpr->res.end(); it1++)
	{
		*size = (*it1).size;

		for( uint64_t j=0; j<(*it1).relationID.size(); j++)
			if( (*it1).relationID.at(j) == relationId )			return (*it1).rowID.at(j);

	}

	return NULL;
}

int tempResultsUpdate(std::vector<uint64_t> &results, int relationId, tempResults *tpr)
{
	std::vector<tempResultArray>::iterator it;

	for(it = tpr->res.begin(); it != tpr->res.end(); it++)								// Iterate through all saved tempresultarrays
	{																					// to find the one containing relation with relationId
		for(uint64_t i=0; i< (*it).relationID.size(); i++)
		{
			if( (*it).relationID.at(i) == relationId )									// If found
			{

				uint64_t k=0;
				std::vector<uint64_t *> newRowId;

				for(uint64_t j=0; j<(*it).relationID.size(); j++)						// Create new tempresults
				{
					uint64_t *newarr = new uint64_t[(*it).size];
					newRowId.push_back(newarr);
				}

				std::vector<uint64_t>::iterator it1;

				uint64_t f=0;
				for(it1 = results.begin(); it1 != results.end(); it1++,f++)
				{
					while( (*it1) != ((*it).rowID.at(i))[k])	k++;					// Go though old temp results
					for(uint64_t j=0; j<(*it).relationID.size(); j++)
						(newRowId.at(j))[f] = (*it).rowID.at(i)[j];						// Copy filtered rows to new temp results
				}

				std::vector<uint64_t *>::iterator tmp;

				for(tmp = (*it).rowID.begin(); tmp != (*it).rowID.end(); tmp++)			// destroy old rowid
					delete *tmp;

				(*it).rowID = newRowId;													// and assign new one to tempresults

				return 0;
			}
		}
	}
		return 1;
}

int tempResultsAdd(std::vector<uint64_t> &results, int relationId, tempResults *tpr)
{
	uint64_t *arr = new uint64_t[results.size()];
	std::vector<uint64_t>::iterator it;

	int i=0;
	for( it = results.begin(); it != results.end() ; it++,i++)
		arr[i] = (*it);

	tempResultArray newTmp;

	newTmp.rowID.push_back(arr);
	newTmp.relationID.push_back(relationId);
	newTmp.size = results.size();

	tpr->res.push_back(newTmp);

	return 0;
}

uint64_t getChecksum(tempResultArray* tr,relationArray* ra,std::vector<checksum_views*> cv)
{
	uint64_t checksum;
	uint64_t i;
	uint64_t row;

	vector<int>::iterator rid;
	vector<checksum_views*>::iterator check;
	vector<uint64_t>::iterator rowit;

	for(check = cv.begin(); check != cv.end(); check++)
	{
		checksum = 0;
		for(rid = tr->relationID.begin(); rid != tr->relationID.end(); rid++)
		{
			if((*check)->rel_views == (*rid)) //relid = check id.get results now
			{
				for(i = 0 ; i < tr->size ; i++)
				//for(rowit = tr->rowID.at((*rid)).start() ; rowit != tr->rowID.at((*rid)).end(); rowit++)
				{
					row = tr->rowID.at((*rid))[i];
					checksum += (ra->relations.at((*check)->rel_views	))->relation[(*check)->rel_cols][row];
				}
				break;
			}
		}

		if(checksum == 0)
			cout << "NULL ";
		else
			cout << checksum << " ";

	}

cout << endl;


	return 0;	//idk temporary
}



void jointest()
{
	cout << "ok" << std::endl;

	uint64_t * rowID1 = new uint64_t[3];
	uint64_t * rowID2 = new uint64_t[5];

	rowID1[0] = 1;
	rowID1[1] = 2;
	rowID1[2] = 3;

	rowID2[0] = 1;
	rowID2[1] = 2;
	rowID2[2] = 3;
	rowID2[3] = 4;
	rowID2[4] = 5;

	uint64_t *arr1 = new uint64_t[3];
	uint64_t *arr2 = new uint64_t[5];

	arr1[0] = 5;
	arr1[1] = 1;
	arr1[2] = 9;

	arr2[0] = 7;
	arr2[1] = 6;
	arr2[2] = 1;
	arr2[3] = 9;
	arr2[4] = 1;

	Table_Info *tableInfo1 = init_table_info(rowID1, arr1, 3);
	Table_Info *tableInfo2 = init_table_info(rowID2, arr2, 5);

	daIndex **indx = DAIndexArrayCreate(tableInfo2->bck_array);

	for(int i=0; i<16; i++)
	{
		cout << "bucket" << i << endl;
		for(int j=0;j<indx[i]->bucket->size;j++)
			cout << indx[i]->bucket->table[j] << endl;
		cout << endl;
	}

	result *res = getResults(tableInfo2,tableInfo1,indx);

	print_results(res);

}


