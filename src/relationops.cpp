#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"
#include "../header/helper_functions.h"
#include "../header/daindex.h"
#include "../header/results.h"
#include "../header/thread_scheduler.h"
#include "../header/joinenum.h"
#include <math.h>
#include <queue>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

threadpool* thread_pool;

tempResults *queryExecute(Query *qr, relationArray *relArray,threadpool* tp)
{
	thread_pool = tp;
	queryReorder(qr);																	// Reorders predicates in query for optimization purposes
	std::vector<predicates*>::iterator it;

	for( it = qr->p.begin(); it != qr->p.end(); it++)
		if( (*it)->type != JOIN )		filtered_relation((*it),relArray);

	Query *qur = editQuery(qr);
	optimizeQuery(relArray,qur);

	tempResults *tRes = new tempResults;

	for( it = qur->p.begin(); it != qur->p.end(); it++)
	{
		if( (*it)->type == JOIN)		relation_join((*it),relArray,tRes);				// Each predicate is either a join or a filter
	}

	return tRes;
}

relationArray *createTempRelArray(relationArray *rArray, Query *qr)
{
	relationArray *newArray = new relationArray;

	for(uint64_t i = 0; i < qr->relations.size(); i++)
	{
		int realID = qr->relations.at(i);

		Relations *originalRel = rArray->relations.at(realID);
		Relations *rel = new Relations;

		rel->size = originalRel->size;
		rel->numColumns = originalRel->numColumns;
		rel->relation   = new uint64_t *[rel->numColumns];

		rel->relationStats = new columnStats[rel->numColumns];
		for(uint64_t j = 0; j < rel->numColumns; j++)
		{
			rel->relation[j] = new uint64_t[rel->size];
			for(uint64_t k = 0; k < rel->size; k++)
				rel->relation[j][k] = originalRel->relation[j][k];

			rel->relationStats[j].minVal = originalRel->relationStats[j].minVal;
			rel->relationStats[j].maxVal = originalRel->relationStats[j].maxVal;
			rel->relationStats[j].valueCount = originalRel->relationStats[j].valueCount;
			rel->relationStats[j].uniqueCount = originalRel->relationStats[j].uniqueCount;

		}


		newArray->relations.push_back(rel);
	}

	return newArray;
}



void deleteTR(tempResults** tr){
	for(uint64_t i = 0 ; i < (*tr)->res.size() ; i++){

			for(uint64_t a = 0 ; a < ((*tr)->res.at(i)).rowID.size() ; a++){
				delete[]((*tr)->res.at(i).rowID.at(a));
			}

			vector<uint64_t*>().swap((*tr)->res.at(i).rowID);
			vector<int>().swap((*tr)->res.at(i).relationID);

	}

	vector<tempResultArray>().swap((*tr)->res);

}

Query *editQuery(Query *qr)																// Removes duplicate predicates
{

	std::vector<predicates *>::iterator it1, it2, it3;

	it1 = qr->p.begin();
	while((*(qr->p.begin()))->type != JOIN )
	{
		delete *(qr->p.begin());
		qr->p.erase(qr->p.begin());
	}

	for( it2 = it1; it2 != qr->p.end(); it2++)
	{
		for(it3 = it2+1; it3 < qr->p.end(); it3++)
		{
			if(isEqualPred((*it2),(*it3)) == 0)
			{
				qr->p.erase(it3);
			}
		}
	}
	return qr;
}


inline uint64_t **convert_to_arrays(result *r,uint64_t &ts){
	uint64_t** r_convd = new uint64_t*[2];

	int total_size = r->results_array.size();


	ts = total_size;
	r_convd[0] = new uint64_t[total_size];
	r_convd[1] = new uint64_t[total_size];

	for(int i = 0 ; i < total_size ; i++){
		r_convd[0][i] = r->results_array.at(i)->key;
		r_convd[1][i] = r->results_array.at(i)->payload;
	}

	return r_convd;

}


void queryReorder(Query *qr)
{
	std::vector<predicates *> predv;

	for(uint64_t i=0; i < qr->p.size(); i++)
		if(qr->p.at(i)->type != JOIN)
			predv.push_back(qr->p.at(i));

	for(uint64_t i=0; i < qr->p.size(); i++)
		if(qr->p.at(i)->type == JOIN)
			predv.push_back(qr->p.at(i));

	qr->p = predv;
}


void filtered_relation(predicates *pred,relationArray* rArray)
{
	int relationId = pred->relation1;
	int columnId = pred->column1;
	uint64_t filter = pred->filter;

	Relations *currentRelation = rArray->relations.at(relationId);
	uint64_t **filtered;

	vector<uint64_t> results;

	for (uint64_t row = 0 ; row < currentRelation->size ; row++)
	{
		uint64_t rid = row;
		switch(pred->type){
			case EQ_FILTER:
				if(filter == currentRelation->relation[columnId][rid])
					results.push_back(rid);
				break;
			case LT_FILTER:
				if(filter > currentRelation->relation[columnId][rid])
					results.push_back(rid);
				break;
			case GT_FILTER:
				if(filter < currentRelation->relation[columnId][rid])
					results.push_back(rid);
				break;
			default:
				break;
		}
	}

	filtered = new uint64_t *[currentRelation->numColumns];

	for (uint64_t i = 0 ; i < currentRelation->numColumns ; i++)
	{
		filtered[i] = new uint64_t [results.size()];
		for(uint64_t j = 0 ; j < results.size() ; j ++)
		{
			uint64_t rid = results.at(j);
			filtered[i][j] = currentRelation->relation[i][rid];
		}

	}

	columnStats *rStats = currentRelation->relationStats;

	switch (pred->type)
	{
		case EQ_FILTER:
		{
			rStats[columnId].minVal = filter;									// ColumnID Stats
			rStats[columnId].maxVal = filter;
			uint64_t original = rStats[columnId].valueCount;
			rStats[columnId].valueCount = results.size();

			if( results.size() == 0 )		rStats[columnId].uniqueCount = 0;
			else							rStats[columnId].uniqueCount = 1;

			for ( uint64_t i = 0 ; i < currentRelation->numColumns ; i++ )
				if( i != columnId )
				{
					float factor = (1 - pow(( 1 - rStats[columnId].valueCount/original ),(rStats[i].valueCount/rStats[i].uniqueCount)));
					rStats[i].uniqueCount = (uint64_t) (rStats[i].uniqueCount)*factor ;
					rStats[i].valueCount = rStats[columnId].valueCount;
				}

			break;
		}
		case GT_FILTER:
		{
			rStats[columnId].uniqueCount = (uint64_t) ( ( (rStats[columnId].maxVal - filter)/(rStats[columnId].maxVal - rStats[columnId].minVal) )*(rStats[columnId].uniqueCount) );
			rStats[columnId].minVal = filter;
			uint64_t original = rStats[columnId].valueCount;
			rStats[columnId].valueCount = results.size();

			for( uint64_t i = 0 ; i < currentRelation->numColumns ; i++)
				if( i != columnId )
				{
					float factor = (1 - pow(( 1 - rStats[columnId].valueCount/original ),(rStats[i].valueCount/rStats[i].uniqueCount)));
					rStats[i].uniqueCount = (uint64_t) (rStats[i].uniqueCount)*factor ;
					rStats[i].valueCount = rStats[columnId].valueCount;
				}

			break;
		}
		case LT_FILTER:
		{
			rStats[columnId].uniqueCount = (uint64_t) ( ( ( filter - rStats[columnId].minVal)/(rStats[columnId].maxVal - rStats[columnId].minVal) )*(rStats[columnId].uniqueCount) );
			rStats[columnId].maxVal = filter;
			uint64_t original = rStats[columnId].valueCount;
			rStats[columnId].valueCount = results.size();

			for( uint64_t i = 0 ; i < currentRelation->numColumns ; i++)
				if( i != columnId )
				{
					float factor = (1 - pow(( 1 - rStats[columnId].valueCount/original ),(rStats[i].valueCount/rStats[i].uniqueCount)));
					rStats[i].uniqueCount = (uint64_t) (rStats[i].uniqueCount)*factor ;
					rStats[i].valueCount = rStats[columnId].valueCount;
				}

			break;
		}
		default:
			break;
	}


	for(uint64_t k = 0; k < currentRelation->numColumns; k++ )
	{
		delete[] currentRelation->relation[k];
	}
	delete[] currentRelation->relation;

	currentRelation->relation = filtered;
	currentRelation->size = results.size();
	currentRelation->relationStats = rStats;

}

void fringeCase(relationArray *rArray, tempResults *tr, int relationId1, int relationId2, int columnId1, int columnId2)
{
	Relations *currRelation1 = rArray->relations.at(relationId1);						// Fetch RELATIONS
	Relations *currRelation2 = rArray->relations.at(relationId2);

	std::vector<int>::iterator it1;
	std::vector<uint64_t *>::iterator it2 = tr->res.at(0).rowID.begin();

	uint64_t *rid1,*rid2;

	for( it1 = tr->res.at(0).relationID.begin(); it1 != tr->res.at(0).relationID.end(); it1++,it2++)
	{
		if( relationId1 == (*it1) )		rid1 = (*it2);
		if( relationId2 == (*it1) )		rid2 = (*it2);
	}


	uint64_t newSize = 0;
	std::vector< std::vector<uint64_t> > newRes;
	newRes.resize(tr->res.at(0).rowID.size());

	for(uint64_t i=0; i < tr->res.at(0).size; i++)
	{
		if( currRelation1->relation[columnId1][rid1[i]] == currRelation2->relation[columnId2][rid2[i]] )
		{
			for(uint64_t j=0; j < tr->res.at(0).rowID.size(); j++)
			{
				newRes.at(j).push_back(tr->res.at(0).rowID.at(j)[i]);
			}
			newSize++;
		}
	}

	std::vector< uint64_t * > finalRes;
	finalRes.resize(newRes.size());

	for(uint64_t i = 0; i < newRes.size(); i++)
	{
		finalRes.at(i) = new uint64_t[newSize];
		for( uint64_t j = 0; j < newSize; j++)
		{
			finalRes.at(i)[j] = newRes.at(i).at(j);
		}
	}

	tr->res.at(0).size = newSize;
	tr->res.at(0).rowID = finalRes;
}


uint64_t *conjurePayload(uint64_t *column, uint64_t *rowID, uint64_t size)
{
	uint64_t *temp = new uint64_t[size];

	for(uint64_t i=0; i<size; i++)
		temp[i] = column[rowID[i]];

	return temp;

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
	rowID2 = tempResultsLookup(tpr,relationId2, &size2);




	if( rowID1 == NULL )
	{
		rowID1 = createRowID(currentRelation1->size);
		size1  = currentRelation1->size;
		tableInfo1 = init_table_info(rowID1,currentRelation1->relation[columnId1],size1,thread_pool);
	}
	else	foundFlag1++;



	if( rowID2 == NULL )
	{
		rowID2 = createRowID(currentRelation2->size);
		size2  = currentRelation2->size;
		tableInfo2 = init_table_info(rowID2,currentRelation2->relation[columnId2],size2,thread_pool);
	}
	else	foundFlag2++;


	uint64_t *payloadColumn1 = NULL;
	uint64_t *payloadColumn2 = NULL;

	if( foundFlag1 == 1 && foundFlag2 == 1)
	{
		fringeCase(rArray,tpr,relationId1,relationId2,columnId1,columnId2);
		return;
	}
	else if(foundFlag1 == 1)
	{
		payloadColumn1 = conjurePayload(rArray->relations.at(relationId1)->relation[columnId1],rowID1,size1);
		tableInfo1 = init_table_info(rowID1,payloadColumn1,size1,thread_pool);

	}
	else if(foundFlag2 == 2)
	{
		payloadColumn2 = conjurePayload(rArray->relations.at(relationId2)->relation[columnId2],rowID2,size2);
		tableInfo2 = init_table_info(rowID2,payloadColumn2,size2,thread_pool);

	}



	int tempind = 0;
	daIndex **indx;
	result **res;
	uint64_t resultSize;
	uint64_t ** joinResults;
	Table_Info* indexed;
	if(foundFlag2)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx,thread_pool,tempind);
		joinResults = convert_to_arrays(res[0],resultSize);

		uint64_t * tmp = joinResults[1];
		joinResults[1] = joinResults[0];
		joinResults[0] = tmp;
		indexed = tableInfo1;
	}
	else if(foundFlag1)
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx,thread_pool,tempind);
		joinResults = convert_to_arrays(res[0],resultSize);
		indexed = tableInfo2;

	}
	else if (size1 < size2 && !foundFlag2 && !foundFlag1)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx,thread_pool,tempind);
		joinResults = convert_to_arrays(res[0],resultSize);

		uint64_t * tmp = joinResults[1];
		joinResults[1] = joinResults[0];
		joinResults[0] = tmp;
		indexed = tableInfo1;
	}
	else if(size2 <= size1  && !foundFlag2 && !foundFlag1)
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx,thread_pool,tempind);
		joinResults = convert_to_arrays(res[0],resultSize);
		indexed = tableInfo2;
	}

	if(foundFlag2 || foundFlag1)
		tempResultsJoinUpdate(joinResults, relationId1, relationId2, foundFlag1, foundFlag2, resultSize, tpr,res[1]);
	else
		tempResultsJoinUpdate(joinResults, relationId1, relationId2, foundFlag1, foundFlag2, resultSize, tpr,NULL);


	destroy_results(&res[0]);
	destroy_results(&res[1]);
	delete[] res;
	delete[] joinResults;

	DAIndexArrayDestroy(indx,indexed->bck_array->size);
	if(indexed = tableInfo2){
		Destroy_Table_Data(&tableInfo1);
		Destroy_Table_Data(&indexed);
	}else{
		Destroy_Table_Data(&tableInfo2);
		Destroy_Table_Data(&indexed);
	}
	if(rowID1!=NULL)
		delete[] rowID1;
	if(rowID2!=NULL)
		delete[] rowID2;
	if(payloadColumn1 != NULL)
		delete[] payloadColumn1;
	if(payloadColumn2!=NULL)
		delete[] payloadColumn2;
}


uint64_t *createRowID(uint64_t rSize)
{
	uint64_t *rowID = new uint64_t[rSize];
	for(uint64_t i=0; i < rSize; i++)	rowID[i] = i;
	return rowID;
}

void tempResultsJoinUpdate(uint64_t ** joinResults,int relationID1, int relationID2, int foundFlag1, int foundFlag2, uint64_t resultSize, tempResults *tpr,result* old)
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
	else if (foundFlag1 == 1 && foundFlag2 ==0)
	{


		for( it = tpr->res.begin(); it != tpr->res.end(); it++)
		{
				std::vector<int>::iterator it1;//relationid
				std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();//swsto
				for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++){
					if( (*it1) == relationID1 )//VRIKA TON PINAKA STA TEMP RESULTS
						break;
				}


				for(int i = 0 ; i < (*it).rowID.size();i++){
					if(relationID1 == (*it).relationID.at(i)) continue;

					uint64_t* newrow = new uint64_t[resultSize];
					uint64_t* og = (*it).rowID.at(i);
					for(int j = 0 ; j < resultSize ; j++){
						newrow[j] = og[old->results_array.at(j)->key];
					}
					delete[] (*it).rowID.at(i);
					(*it).rowID.at(i) = newrow;

				}

				(*it2)= joinResults[0];
				(*it).relationID.push_back(relationID2);
				(*it).rowID.push_back(joinResults[1]);
				(*it).size = resultSize;
		}
	}
	else if(foundFlag2==1 && foundFlag1 ==0)
	{
		for( it = tpr->res.begin(); it != tpr->res.end(); it++)
		{
				std::vector<int>::iterator it1;
				std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
				for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++)
					if( (*it1) == relationID2 )//VRIKA TON PINAKA STA TEMP RESULTS
						break;


				for(int i = 0 ; i < (*it).rowID.size();i++){
					if(relationID2 == (*it).relationID.at(i)) continue;

					uint64_t* newrow = new uint64_t[resultSize];
					uint64_t* og = (*it).rowID.at(i);
					for(int j = 0 ; j < resultSize ; j++){
						newrow[j] = og[old->results_array.at(j)->key];
					}
					delete[] (*it).rowID.at(i);
					(*it).rowID.at(i) = newrow;

				}

				//delete[] (*it2);
				(*it2) = joinResults[1];
				(*it).relationID.push_back(relationID1);
				(*it).rowID.push_back(joinResults[0]);
				(*it).size = resultSize;
		}
	}
	return;
}


inline uint64_t *tempResultsLookup(tempResults *tpr, int relationId, uint64_t *size)
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

				std::vector<uint64_t *> newRowId;

				for(uint64_t j=0; j<(*it).relationID.size(); j++)						// Create new tempresults
				{
					uint64_t *newarr = new uint64_t[(*it).size];
					newRowId.push_back(newarr);
				}

				for(uint64_t l = 0; l < (*it).size; l++)
				{
					uint64_t chk = (*it).rowID.at(i)[l];
					uint64_t f=0;

					std::vector<uint64_t>::iterator it1;
					for( it1 = results.begin(); it1 != results.end(); it1++,f++)
					{
						if((*it1) == chk)
						{
							for( uint64_t j=0; j<(*it).relationID.size(); j++)
								newRowId.at(j)[f] = (*it).rowID.at(j)[l];
						}
					}
				}

				std::vector<uint64_t *>::iterator tmp;

				for(tmp = (*it).rowID.begin(); tmp != (*it).rowID.end(); tmp++)			// destroy old rowid
					delete *tmp;

				(*it).rowID = newRowId;													// and assign new one to tempresults
				(*it).size  = results.size();
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
		{
			if((*check)->rel_views == tr->relationID.at(j))
			{
				for(i = 0 ; i < tr->size ; i++)
				{
					uint64_t* temp = tr->rowID.at(j);
					row = temp[i];
					relID = tr->relationID.at(j);
					checksum += (ra->relations.at(relID))->relation[(*check)->rel_cols][row];
				}

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






int isEqualPred(predicates *p1, predicates *p2)
{
	if( p1->type != p2->type )	return 1;

	if( p1->type == JOIN )
	{
		if( p1->relation1 == p2->relation1)
		{
			if( p1->relation2 == p2->relation2 && p1->column1 == p2->column1 && p1->column2 == p2->column2 )
				return 0;
		}
		else if( p1->relation1 == p2->relation2)
		{
			if( p1->relation2 == p2->relation1 && p1->column2 == p2->column1 && p1->column1 == p2->column2 )
				return 0;
		}
	}
	else
	{
		if(p1->relation1 == p2->relation1 && p1->column1 == p2->column1 && p1->filter == p2->filter)
			return 0;
	}

	return 1;
}


void printJoinResults(uint64_t ** joinResults, relationArray *rarr, int r1, int r2, int rsize)
{
	for(int i = 0; i < rsize; i++)
	{
		for(uint64_t j = 0; j < rarr->relations.at(r1)->numColumns; j++)
		{
			cout << rarr->relations.at(r1)->relation[j][joinResults[0][i]] << " | ";
		}

		for(uint64_t j = 0; j < rarr->relations.at(r2)->numColumns; j++)
		{
			cout << rarr->relations.at(r2)->relation[j][joinResults[1][i]] << " | ";
		}

		cout << endl;
	}
}


void queryPrint(Query *q)
{
	std::vector<predicates *>::iterator it;
	for(it = q->p.begin(); it != q->p.end(); it++ )
		if((*it)->type == JOIN)
			cout << "JOINING RELATIONS " << (*it)->relation1 << " AND " << (*it)->relation2 << " ON COLUMNS " << (*it)->column1 << " AND " << (*it)->column2 << endl;
		else
		{
			cout << "QUERYING RELATION " << (*it)->relation1 << " FOR ROWS WHERE ELEMENT IN COLUMN " << (*it)->column1;
			if((*it)->type == EQ_FILTER)			cout << " IS EQUAL ";
			else if((*it)->type == GT_FILTER)		cout << " IS GREATER THAN ";
			else if((*it)->type == LT_FILTER)		cout << " IS LESS THAN ";
			else
			{
				cout << "why" << endl;
				exit(0);
			}
			cout << (*it)->filter << endl;
		}
}


void printTPR(tempResults *tpr, relationArray *rarr)
{
	std::vector<tempResultArray>::iterator it;
	for( it = tpr->res.begin(); it != tpr->res.end(); it++ )
	{
		for(uint64_t j = 0; j < (*it).size; j++)
		{
			std::vector<uint64_t *>::iterator it2;
			std::vector<int>::iterator it3 = (*it).relationID.begin();
			for(it2 = (*it).rowID.begin(); it2 != (*it).rowID.end(); it2++, it3++)
			{
				Relations *tr = rarr->relations.at(*it3);

				for(uint64_t i = 0; i < tr->numColumns; i++)
				{
					cout << tr->relation[i][(*it2)[j]] << " |";
				}
				cout << "|";

			}
			cout << endl;
		}

	}
}

/*

Query *queryReorder(Query *qr)
{
	Query *newQr = new Query;
	int predCount = qr->p.size();
	queryHelper *qHelp = new queryHelper[predCount];

	std::vector<predicates *>::iterator it;

	int i=0;
	uint64_t joinCount=0;
	for( it = qr->p.begin(); it != qr->p.end(); it++,i++)
	{
		if((*it)->type == JOIN)
		{
			qHelp[i].relation1 = (*it)->relation1;
			qHelp[i].relation2 = (*it)->relation2;
			qHelp[i].p = (*it);
			joinCount++;
		}
	}

	if(joinCount != 0)																	// bfs
	{
		std::queue<int> q;
		std::vector<int> visited;

		q.push(qHelp[0].relation1);
		if(qHelp[0].relation2 != qHelp[0].relation1)	q.push(qHelp[0].relation2);
		newQr->p.push_back(qHelp[0].p);

		while( !q.empty() && newQr->p.size() != joinCount )
		{
			int front = q.front();
			visited.push_back(front);
			q.pop();

			for(uint64_t j=0 ; j < joinCount; j++)
			{
				if(qHelp[j].relation1 == front || qHelp[j].relation2 == front)
				{
					int flg1=0;
					for(uint64_t k=0 ; k < newQr->p.size() ; k++)
					{
						if(qHelp[j].p == newQr->p.at(k))	flg1=1;
					}

					if(flg1==0)	newQr->p.push_back(qHelp[j].p);


					flg1=0;
					if( qHelp[j].relation1 == front )
					{
						for(uint64_t k=0; k< visited.size(); k++)
						{
							if(visited.at(k) == qHelp[j].relation2)	flg1=1;
						}
						if(flg1==0)	q.push(qHelp[j].relation2);
					}
					else
					{
						for(uint64_t k=0; k< visited.size(); k++)
						{
							if(visited.at(k) == qHelp[j].relation1)	flg1=1;
						}
						if(flg1==0)	q.push(qHelp[j].relation1);
					}
				}
			}
		}


	}




	for(  it = qr->p.begin(); it != qr->p.end(); it++ )
		if( (*it)->type != JOIN )
			newQr->p.push_back(*it);

	newQr->relations = qr->relations;
	newQr->checksums = qr->checksums;
	return newQr;
}

*/


/*

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
			// cout << "RID is "  << rid << endl;
			switch (pred->type) {
				case EQ_FILTER:
					if( filter == currentRelation->relation[columnId][rid])
						results.push_back(rid);
					break;

				case GT_FILTER:
					if( filter < currentRelation->relation[columnId][rid])
						results.push_back(rid);
					break;

				case LT_FILTER:
					if( filter > currentRelation->relation[columnId][rid])
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
					if( filter < currentRelation->relation[columnId][row])
						results.push_back(row);
					break;

				case LT_FILTER:
					if( filter > currentRelation->relation[columnId][row])
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

*/

/*
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
*/


