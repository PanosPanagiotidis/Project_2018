#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"
#include "../header/helper_functions.h"
#include "../header/daindex.h"
#include "../header/results.h"

#include <iostream>

using namespace std;

tempResultArray *queryExecute(Query *qr, relationArray *relArray)
{
	Query *orderedQuery = queryReorder(qr);												// Reorders predicates in query for optimization purposes
	tempResults *tRes = new tempResults;

	std::vector<predicates*>::iterator it;
	for( it = qr->p.begin(); it != qr->p.end(); it++){
		if( (*it)->type == JOIN)		continue;										// Each predicate is either a join or a filter
		else	relation_filter((*it),relArray,tRes);
	}
	delete orderedQuery;

	return &(tRes->res.at(0));
}

uint64_t **convert_to_arrays(result *r,uint64_t &ts){
	uint64_t** r_convd = new uint64_t*[2];

	result *temp=r;
	int total_size = 0;
	int count = 0;

	while(temp->next!=NULL){
		total_size += temp->size;
		temp = temp->next;
	}

	ts = total_size;

	for(int i = 0 ; i < total_size ; i++){
		r_convd[0] = new uint64_t[total_size];
		r_convd[1] = new uint64_t[total_size];

		r_convd[0][i] = r->results_array[count].key;
		r_convd[2][i] = r->results_array[count].payload;

		count++;

		if(count > r->size && r->next!=NULL)
			r = r->next;
		else break;
	}

	return r_convd;

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

		tempResultsFilterUpdate(results,relationId,tr);
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
		tempResultsAdd(results,relationId,tr);
	}
}


void specialCase(relationArray *rArray, tempResults *tr, int relationId, int columnId1, int columnId2)
{
	Relations * currentRelation = rArray->relations.at(relationId);						// Fetch relation

	uint64_t *size = new uint64_t;
	uint64_t *rowids = tempResultsLookup(tr, relationId, size);							// Also check if relation exists on tempResults

	std::vector<uint64_t> results;														// Result vector

	if(rowids != NULL )																	// If relation exists on tempResults
	{

		for(uint64_t row=0; row < *size; row++)
		{
			uint64_t rid = rowids[row];
			if( currentRelation->relation[columnId1][rid] == currentRelation->relation[columnId2][rid])
				results.push_back(rid);
		}
		tempResultsFilterUpdate(results,relationId,tr);
	}
	else
	{
		for(uint64_t row=0; row < currentRelation->size; row++)
		{
			if( currentRelation->relation[columnId1][row] == currentRelation->relation[columnId2][row])
				results.push_back(row);
		}
	}
	tempResultsAdd(results,relationId,tr);
}


void relation_join(predicates *pred, relationArray *rArray, tempResults *tpr)
{
	int relationId1 = pred->relation1;
	int relationId2 = pred->relation2;
	int columnId1   = pred->column1;
	int columnId2   = pred->column2;

	uint64_t size1, size2;
	uint64_t *rowID1, *rowID2;
	Table_Info *tableInfo1, *tableInfo2;
	int foundFlag1 = 0, foundFlag2 = 0;

	Relations * currentRelation1 = rArray->relations.at(relationId1);
	Relations * currentRelation2 = rArray->relations.at(relationId2);


	rowID1 = tempResultsLookup(tpr,relationId1, &size1);

	if( rowID1 == NULL )
	{
		rowID1 = createRowID(currentRelation1->size);
		size1  = currentRelation1->size;
		tableInfo1 = init_table_info(rowID1,currentRelation1->relation[columnId1],size1);
	}
	else
	{
		foundFlag1++;
		uint64_t *payloadColumn1 = rArray->relations.at(relationId1)->relation[columnId1];
		tableInfo1 = init_table_info(rowID1,payloadColumn1,size1);
	}


	rowID2 = tempResultsLookup(tpr,relationId2, &size2);

	if( rowID2 == NULL )
	{
		rowID2 = createRowID(currentRelation2->size);
		size2  = currentRelation2->size;
		tableInfo2 = init_table_info(rowID2,currentRelation2->relation[columnId2],size2);
	}
	else
	{
		foundFlag2++;
		uint64_t *payloadColumn2 = rArray->relations.at(relationId2)->relation[columnId2];
		tableInfo2 = init_table_info(rowID2,payloadColumn2,size2);
	}

	if(relationId1 == relationId2)														// fringe case  relation1 == relation2, run a eq_filter where a.x = a.y
	{
		specialCase(rArray,tpr,relationId1,columnId1,columnId2);
		return;
	}

	daIndex **indx;
	result *res;

	if(size1 < size2)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx);
	}
	else
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx);
	}

	uint64_t resultSize;
	uint64_t ** joinResults = convert_to_arrays(res,resultSize);

	tempResultsJoinUpdate(joinResults, relationId1, relationId2, foundFlag1, foundFlag2, resultSize, tpr);
}


uint64_t *createRowID(uint64_t rSize)
{
	uint64_t *rowID = new uint64_t[rSize];
	for(uint64_t i=0; i < rSize; i++)	rowID[i] = i;
	return rowID;
}

void tempResultsJoinUpdate(uint64_t ** joinResults,int relationID1, int relationID2, int foundFlag1, int foundFlag2, uint64_t resultSize, tempResults *tpr)
{
	std::vector<tempResultArray>::iterator it;

	if( foundFlag1 == 0 && foundFlag2 == 0)												// Neither relation exists on tempresults
	{																					// Create a new tempresult array with both
		tempResultArray temp;

		temp.rowID.push_back(joinResults[0]);
		temp.rowID.push_back(joinResults[1]);

		temp.relationID.push_back(relationID1);
		temp.relationID.push_back(relationID2);

		temp.size = resultSize;

		tpr->res.push_back(temp);

	}
	else if( foundFlag1 == 1 && foundFlag2 == 0)										// Only Relation 1  exists on tempresults
	{
		for( it = tpr->res.begin(); it != tpr->res.end(); it++)
		{
			std::vector<int>::iterator it1;
			std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
			for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++)
				if( (*it1) == relationID1 )
				{
					std::vector< std::vector<uint64_t> > matrix;						// Create a 2d array
					matrix.resize((*it).relationID.size()+1);							// with as many columns as tempresult struct plus one to fit new column

					for(uint64_t i = 0; i < resultSize; i++)
					{
						for(uint64_t j = 0; j < (*it).relationID.size(); j++ )
						{
							if( joinResults[0][i] == (*it2)[j] )
							{
								uint64_t k;
								for(k = 0; k < matrix.size()-1; k++)
									(matrix.at(k)).push_back(((*it).rowID.at(k))[j]);
								matrix.at(k).push_back(joinResults[1][i]);
							}
						}
					}

					std::vector<uint64_t *> newRowID;
					uint64_t newSize;

					for(uint64_t i = 0; i < matrix.size(); i++)
					{
						newSize = (matrix.at(i)).size();
						uint64_t *temp = new uint64_t[newSize];

						for(uint64_t k = 0; k < newSize; k++)
							temp[k] = (matrix.at(i)).at(k);

						newRowID.push_back(temp);
					}

					(*it).relationID.push_back(relationID2);
					(*it).size = newSize;
																						// TODO: cleanup the memory mess
																						// before doing this:
					(*it).rowID = newRowID;
					return;																// Just once
				}

		}

	}
	else if( foundFlag1 == 0 && foundFlag2 == 1)										// Only Relation 2  exists on tempresults
	{
		for( it = tpr->res.begin(); it != tpr->res.end(); it++)
		{
			std::vector<int>::iterator it1;
			std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
			for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++)
				if( (*it1) == relationID2 )
				{
					std::vector< std::vector<uint64_t> > matrix;						// Create a 2d array
					matrix.resize((*it).relationID.size()+1);							// with as many columns as tempresult struct plus one to fit new column

					for(uint64_t i = 0; i < resultSize; i++)
					{
						for(uint64_t j = 0; j < (*it).relationID.size(); j++ )
						{
							if( joinResults[1][i] == (*it2)[j] )
							{
								uint64_t k;
								for(k = 0; k < matrix.size()-1; k++)
									(matrix.at(k)).push_back(((*it).rowID.at(k))[j]);
								matrix.at(k).push_back(joinResults[0][i]);
							}
						}
					}

					std::vector<uint64_t *> newRowID;
					uint64_t newSize;

					for(uint64_t i = 0; i < matrix.size(); i++)
					{
						newSize = (matrix.at(i)).size();
						uint64_t *temp = new uint64_t[newSize];

						for(uint64_t k = 0; k < newSize; k++)
							temp[k] = (matrix.at(i)).at(k);

						newRowID.push_back(temp);
					}

					(*it).relationID.push_back(relationID1);
					(*it).size = newSize;
																						// TODO: cleanup the memory mess
																						// before doing this:
					(*it).rowID = newRowID;
					return;																// Just once
				}
		}
	}
	else
	{
		// Maybe check if both in same temmpResultArray for debug test if something comes up

		for( it = tpr->res.begin(); it != tpr->res.end(); it++)
		{
			std::vector<int>::iterator it1;
			std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
			uint64_t  m = 0, relationPos1, relationPos2;

			for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++,m++)
			{
				if((*it1) == relationID1)	relationPos1 = m;
				if((*it1) == relationID2)	relationPos2 = m;
			}

			std::vector< std::vector<uint64_t> > matrix;
			matrix.resize((*it).relationID.size());

			for(uint64_t i = 0; i < resultSize; i++)
			{
				for(uint64_t j = 0; j < (*it).relationID.size(); j++)
				{
					if( joinResults[0][i] == ((*it).rowID.at(relationPos1))[j] && joinResults[1][i] == ((*it).rowID.at(relationPos2))[j] )
					{
						uint64_t k;
						for(k = 0; k < matrix.size(); k++)
							(matrix.at(k)).push_back(((*it).rowID.at(k))[j]);
					}
				}
			}

			std::vector<uint64_t *> newRowID;

			for(uint64_t i = 0; i < matrix.size(); i++)
			{
				uint64_t *temp = new uint64_t[matrix.size()];

				for(uint64_t k = 0; k < matrix.size(); k++)
					temp[k] = (matrix.at(i)).at(k);

				newRowID.push_back(temp);
			}
																				// TODO: cleanup the memory mess
																				// before doing this:
			(*it).rowID = newRowID;
			return;																// Just once
		}
	}
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

int tempResultsFilterUpdate(std::vector<uint64_t> &results, int relationId, tempResults *tpr)
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
	int relID;

	vector<int>::iterator rid;
	vector<checksum_views*>::iterator check;
	vector<uint64_t>::iterator rowit;
	uint64_t j;

	for(check = cv.begin(); check != cv.end(); check++)
	{	checksum = 0;
		for(j = 0; j < tr->relationID.size() ; j++)
		//for(rid = tr->relationID.begin(); rid != tr->relationID.end(); rid++)
		{
			if((*check)->rel_views == tr->relationID.at(j)) //relid = check id.get results now
			{
				//cout << (*rid) << " is rid" <<endl;
				// cout << "size is " << tr->rowID.size()<< endl;
				for(i = 0 ; i < tr->size ; i++)
				//for(rowit = tr->rowID.at((*rid)).start() ; rowit != tr->rowID.at((*rid)).end(); rowit++)
				{
					cout << "rowID size is " << tr->rowID.size()<<endl;
					uint64_t* temp = tr->rowID.at(j);
					row = temp[i];
					relID = tr->relationID.at(j);
					// cout << (*check)->rel_cols << " rel cols" << endl;
					// cout << row << " row" << endl;
					//cout << "number added is " << (ra->relations.at(relID))->relation[(*check)->rel_cols][row] << endl;
					checksum += (ra->relations.at(relID))->relation[(*check)->rel_cols][row];
				}
				break;
			}
		}

		if(checksum == 0)
			cout << "NULL ";
		else
			cout << "checksum is "<< checksum << " ";

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


