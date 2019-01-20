#include "parser.h"
#include "relationops.h"
#include "relation_loader.h"
#include "helper_functions.h"
#include "daindex.h"
#include "results.h"
#include "thread_scheduler.h"
#include "joinenum.h"
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

	for( it = (qur)->p.begin(); it != (qur)->p.end(); it++)
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

			for(uint64_t a = 0 ; a < (*tr)->res[i].rowID.size() ; a++){
				delete[]((*tr)->res[i].rowID[a]);
			}

			vector<uint64_t*>().swap((*tr)->res[i].rowID);
			vector<int>().swap((*tr)->res[i].relationID);

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
				delete *it3;
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
		r_convd[0][i] = r->results_array[i]->key;
		r_convd[1][i] = r->results_array[i]->payload;
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

	Relations *currentRelation = rArray->relations[relationId];
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
			uint64_t rid = results[j];
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
	Relations *currRelation1 = rArray->relations[relationId1];						// Fetch RELATIONS
	Relations *currRelation2 = rArray->relations[relationId2];

	std::vector<int>::iterator it1;
	std::vector<uint64_t *>::iterator it2 = tr->res[0].rowID.begin();

	uint64_t *rid1,*rid2;

	for( it1 = tr->res[0].relationID.begin(); it1 != tr->res[0].relationID.end(); it1++,it2++)
	{
		if( relationId1 == (*it1) )		rid1 = (*it2);
		if( relationId2 == (*it1) )		rid2 = (*it2);
	}


	uint64_t newSize = 0;
	std::vector< std::vector<uint64_t> > newRes;
	newRes.resize(tr->res[0].rowID.size());

	for(uint64_t i=0; i < tr->res[0].size; i++)
	{
		if( currRelation1->relation[columnId1][rid1[i]] == currRelation2->relation[columnId2][rid2[i]] )
		{
			for(uint64_t j=0; j < tr->res[0].rowID.size(); j++)
			{
				newRes[j].push_back(tr->res[0].rowID.at(j)[i]);
			}
			newSize++;
		}
	}

	std::vector< uint64_t * > finalRes;
	finalRes.resize(newRes.size());

	for(uint64_t i = 0; i < newRes.size(); i++)
	{
		finalRes[i] = new uint64_t[newSize];
		for( uint64_t j = 0; j < newSize; j++)
		{
			finalRes.at(i)[j] = newRes.at(i).at(j);
		}
		delete[] tr->res[0].rowID.at(i);
	}

	tr->res[0].size = newSize;
	tr->res[0].rowID = finalRes;
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

	Relations * currentRelation1 = rArray->relations[relationId1];
	Relations * currentRelation2 = rArray->relations[relationId2];

	rowID1 = tempResultsLookup(tpr,relationId1, &size1);
	rowID2 = tempResultsLookup(tpr,relationId2, &size2);


	if( rowID1 == NULL && size1 == 0 )
	{
		rowID1 = createRowID(currentRelation1->size);
		size1  = currentRelation1->size;
		tableInfo1 = init_table_info2(&rowID1, currentRelation1->relation[columnId1], 0, 0, size1, thread_pool);
	}
	else if( rowID1 == NULL && size1 == 1)												// signifies that it was found and it was empty
	{
		tpr->res[0].rowID.push_back(NULL);
		tpr->res[0].relationID.push_back(relationId2);
		tpr->res[0].size = 0;
		return;
	}
	else
		foundFlag1++;


	if( rowID2 == NULL && size2 == 0)
	{
		rowID2 = createRowID(currentRelation2->size);
		size2  = currentRelation2->size;
		tableInfo2 = init_table_info2(&rowID2, currentRelation2->relation[columnId2], 0, 0, size2, thread_pool);
	}
	else if( rowID2 == NULL && size2 == 1)
	{
		tpr->res[0].rowID.push_back(NULL);
		tpr->res[0].relationID.push_back(relationId1);
		tpr->res[0].size = 0;
		return;
	}
	else
		foundFlag2++;


	uint64_t *payloadColumn1 = NULL;
	uint64_t *payloadColumn2 = NULL;
	uint64_t **DrowID1 = NULL;
	uint64_t **DrowID2 = NULL;
	uint64_t tprPos1, tprPos2;


	if( foundFlag1 == 1 && foundFlag2 == 1)
	{
		fringeCase(rArray,tpr,relationId1,relationId2,columnId1,columnId2);
		return;
	}
	else if(foundFlag1 == 1)
	{
		payloadColumn1 = conjurePayload(rArray->relations[relationId1]->relation[columnId1],rowID1,size1);

		DrowID1 = new uint64_t*[ tpr->res.at(0).rowID.size() ];
		for( uint64_t f = 0; f <  tpr->res.at(0).rowID.size(); f++ )
		{
			DrowID1[f] = tpr->res.at(0).rowID[f];
			if( relationId1 == tpr->res.at(0).relationID[f] )
				tprPos1 = f;
		}

		tableInfo1 = init_table_info2(DrowID1, payloadColumn1,(int) tpr->res.at(0).rowID.size(), tprPos1, size1, thread_pool);
		rowID1 = NULL;

	}
	else if(foundFlag2 == 1)
	{
		payloadColumn2 = conjurePayload(rArray->relations[relationId2]->relation[columnId2],rowID2,size2);

		DrowID2 = new uint64_t*[ tpr->res.at(0).rowID.size() ];
		for( uint64_t f = 0; f <  tpr->res.at(0).rowID.size(); f++ )
		{
			DrowID2[f] = tpr->res.at(0).rowID.at(f);
			if( relationId2 == tpr->res.at(0).relationID.at(f) )
				tprPos2 = f;
		}
		tableInfo2 = init_table_info2(DrowID2, payloadColumn2,(int) tpr->res.at(0).rowID.size(), tprPos2, size2, thread_pool);
		rowID2 = NULL;
	}

	int tempind = 0;
	daIndex **indx;
	result **res;
	Table_Info* indexed;

	if(foundFlag2)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx,thread_pool,tempind);
		indexed = tableInfo1;

		tempResultsJoinUpdate2(res[0], tpr, relationId1, relationId2);
	}
	else if(foundFlag1)
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx,thread_pool,tempind);
		indexed = tableInfo2;

		tempResultsJoinUpdate2(res[0], tpr, relationId2, relationId1);
	}
	else if (size1 < size2 && !foundFlag2 && !foundFlag1)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx,thread_pool,tempind);
		indexed = tableInfo1;

		tempResultsJoinUpdate2(res[0], tpr, relationId2, relationId1);
	}
	else if(size2 <= size1  && !foundFlag2 && !foundFlag1)
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx,thread_pool,tempind);
		indexed = tableInfo2;

		tempResultsJoinUpdate2(res[0], tpr, relationId1, relationId2);
	}


	destroy_results(&res[0]);
	delete[] res;

	DAIndexArrayDestroy(indx,indexed->bck_array->size);
	if(indexed == tableInfo2){
		Destroy_Table_Data(&tableInfo1);
		Destroy_Table_Data(&indexed);
	}else{
		Destroy_Table_Data(&tableInfo2);
		Destroy_Table_Data(&indexed);
	}

	if(DrowID1!=NULL)
	{
		delete[] DrowID1;
	}
	else
	if(DrowID2!=NULL)
	{
		delete[] DrowID2;
	}
	if(rowID1!=NULL)
		delete[] rowID1;
	if(rowID2!=NULL )
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

void tempResultsJoinUpdate2(result *res , tempResults *tpr, int relID1, int relID2)
{
	if( res->results_array.size() > 0 )													// If results not empty
	{
		if( tpr->res.size() > 0 )														// If there are temp results
		{
			uint64_t newtprsize = tpr->res.at(0).rowID.size()+1;
			uint64_t **rsl = new uint64_t*[newtprsize];

			for( uint64_t i = 0; i < newtprsize; i++ )
				rsl[i] = new uint64_t[res->results_array.size()];

			int f = 0;
			for( std::vector<toumble *>::iterator it2 = res->results_array.begin(); it2 != res->results_array.end(); it2++,f++)
			{
				uint64_t i;
				for( i = 0; i < newtprsize-1; i++)
				{
					rsl[i][f] = (*it2)->rids[i];
				}
				rsl[i][f] = (*it2)->payload;
			}

			std::vector<uint64_t *> newRowId;
			for( uint64_t i = 0; i < newtprsize; i++ )
				newRowId.push_back(rsl[i]);


			tpr->res.at(0).relationID.push_back(relID1);
			tpr->res.at(0).size = res->results_array.size();
			for( uint64_t i =0; i < tpr->res.at(0).rowID.size(); i++)
			{
				delete[] tpr->res.at(0).rowID.at(i);
			}
			tpr->res.at(0).rowID = newRowId;
			delete[] rsl;

		}
		else
		{
			uint64_t *rsl[2];
			rsl[0] = new uint64_t[res->results_array.size()];
			rsl[1] = new uint64_t[res->results_array.size()];

			int f = 0;
			for( std::vector<toumble *>::iterator it2 = res->results_array.begin(); it2 != res->results_array.end(); it2++,f++)
			{
				rsl[0][f] = (*it2)->key;
				rsl[1][f] = (*it2)->payload;
			}

			tempResultArray temp;

			temp.rowID.push_back(rsl[0]);
			temp.rowID.push_back(rsl[1]);
			temp.relationID.push_back(relID1);
			temp.relationID.push_back(relID2);

			temp.size = res->results_array.size();
			tpr->res.push_back(temp);
		}
	}
	else
	{
		if( tpr->res.size() > 0 )
		{
			for( std::vector<uint64_t *>:: iterator it = tpr->res.at(0).rowID.begin(); it != tpr->res.at(0).rowID.end(); it++ )
			{
				delete[] *it;
				*it = NULL;
			}

			tpr->res.at(0).relationID.push_back(relID2);
			tpr->res.at(0).rowID.push_back(NULL);
			tpr->res.at(0).size = 0;
		}
		else
		{
			tempResultArray temp;
			temp.rowID.push_back(NULL);
			temp.rowID.push_back(NULL);
			temp.relationID.push_back(relID1);
			temp.relationID.push_back(relID2);

			temp.size = 0;
			tpr->res.push_back(temp);

		}
	}
}

inline uint64_t *tempResultsLookup(tempResults *tpr, int relationId, uint64_t *size)
{
	*size = 0;
	if( tpr == NULL )	return NULL;

	if( tpr->res.size() == 0 )	return NULL;

	std::vector<tempResultArray>::iterator it1;

	for(it1 = tpr->res.begin(); it1 != tpr->res.end(); it1++)
	{
		*size = (*it1).size;
		if(*size == 0)	*size = 1;

		for( uint64_t j=0; j<(*it1).relationID.size(); j++)
			if( (*it1).relationID[j] == relationId )			return (*it1).rowID[j];

	}
	*size = 0;
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
	int count = 0;
	int tcv = cv.size();
	for(check = cv.begin(); check != cv.end(); check++)
	{
		count++;
		checksum = 0;
		for(j = 0; j < tr->relationID.size() ; j++)
		{
			if((*check)->rel_views == tr->relationID[j])
			{
				for(i = 0 ; i < tr->size ; i++)
				{
					uint64_t* temp = tr->rowID[j];
					row = temp[i];
					relID = tr->relationID[j];
					checksum += (ra->relations[relID])->relation[(*check)->rel_cols][row];
				}

			}
		}

		if(checksum == 0)
			cout << "NULL";
		else
			cout << checksum;
		if(count<tcv)
			cout <<" ";

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
		for(uint64_t j = 0; j < rarr->relations[r1]->numColumns; j++)
		{
			cout << rarr->relations[r1]->relation[j][joinResults[0][i]] << " | ";
		}

		for(uint64_t j = 0; j < rarr->relations[r2]->numColumns; j++)
		{
			cout << rarr->relations[r2]->relation[j][joinResults[1][i]] << " | ";
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

