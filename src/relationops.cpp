#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"
#include "../header/helper_functions.h"
#include "../header/daindex.h"
#include "../header/results.h"
#include "../header/thread_scheduler.h"
#include "../header/joinenum.h"
#include <queue>
#include <iostream>
#include <bits/stdc++.h>
#include <math.h>

using namespace std;

threadpool* thread_pool;

tempResults *queryExecute(Query *qr, relationArray *relArray,threadpool* tp)
{
	thread_pool = tp;
	queryReorder(qr);																	// Reorders predicates in query for optimization purposes
	optimizeQuery(relArray,qr);

	tempResults *tRes = new tempResults;
	Query *qur = editQuery(qr);

	// TODO: order of operations:
	//			filters -> optimizequery -> joins
	//


	std::vector<predicates*>::iterator it;
	for( it = qur->p.begin(); it != qur->p.end(); it++)
	{
		if( (*it)->type == JOIN)		relation_join((*it),relArray,tRes);				// Each predicate is either a join or a filter
		else							filtered_relation((*it),relArray);
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

void optimizeQuery(relationArray *rArray, Query *qr)
{
	std::vector<int> *v = joinEnumeration(rArray,qr);
	std::vector<predicates *> newp;

	// TODO: Actually change qr order


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
	while((*it1)->type != JOIN )	it1++;

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


uint64_t **convert_to_arrays(result *r,uint64_t &ts){
	uint64_t** r_convd = new uint64_t*[2];

	result *temp=r;
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

	filtered = (uint64_t**)malloc(sizeof(uint64_t*)*currentRelation->numColumns);

	for (uint64_t i = 0 ; i < currentRelation->numColumns ; i++)
	{
		filtered[i] = (uint64_t*)malloc(sizeof(uint64_t)*(results.size()));
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




	currentRelation->relation = filtered;
	currentRelation->size = results.size();


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

	//cout << "Join: " << relationId1 << " and " << relationId2 << endl;

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



	if( foundFlag1 == 1 && foundFlag2 == 1)
	{
		fringeCase(rArray,tpr,relationId1,relationId2,columnId1,columnId2);
		return;
	}
	else if(foundFlag1 == 1)
	{
		uint64_t *payloadColumn1 = conjurePayload(rArray->relations.at(relationId1)->relation[columnId1],rowID1,size1);
		tableInfo1 = init_table_info(rowID1,payloadColumn1,size1,thread_pool);
	}
	else
	{
		uint64_t *payloadColumn2 = conjurePayload(rArray->relations.at(relationId2)->relation[columnId2],rowID2,size2);
		tableInfo2 = init_table_info(rowID2,payloadColumn2,size2,thread_pool);
	}



	daIndex **indx;
	result *res;
	uint64_t resultSize;
	uint64_t ** joinResults;
	Table_Info* indexed;
	if(size1 < size2)
	{
		indx = DAIndexArrayCreate(tableInfo1->bck_array);
		res = getResults(tableInfo1,tableInfo2,indx,thread_pool);
		joinResults = convert_to_arrays(res,resultSize);

		uint64_t * tmp = joinResults[1];
		joinResults[1] = joinResults[0];
		joinResults[0] = tmp;
		indexed = tableInfo1;
	}
	else
	{
		indx = DAIndexArrayCreate(tableInfo2->bck_array);
		res = getResults(tableInfo2,tableInfo1,indx,thread_pool);
		joinResults = convert_to_arrays(res,resultSize);
		indexed = tableInfo2;

	}



	//cout << "results count: " << resultSize << endl;
	tempResultsJoinUpdate(joinResults, relationId1, relationId2, foundFlag1, foundFlag2, resultSize, tpr);

	//printJoinResults(joinResults, rArray, relationId1, relationId2, resultSize);


	delete[] (rowID2);
	delete[] (rowID1);

	//destroy_results(&res);
	delete(res);
	DAIndexArrayDestroy(indx,indexed->bck_array->size);
	Destroy_Table_Data(&tableInfo1);
	Destroy_Table_Data(&tableInfo2);
	//cout << endl << "JOIN:" << endl << endl;
	//printJoinResults(joinResults,rArray,relationId1,relationId2,resultSize);
	//printTPR(tpr,rArray);
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


					unordered_map<uint64_t,uint64_t> disCount1;							// Unordered map to count appearances of each rowid in joinresults
					for(uint64_t i = 0; i < resultSize; i++)
						disCount1[joinResults[0][i]]++;



					for(uint64_t i = 0; i < resultSize; i++)
						if( disCount1[joinResults[0][i]] == 1 )
						{
							for(uint64_t j = 0; j < (*it).size; j++ )
							{
								if( joinResults[0][i] == (*it2)[j] )
								{

									uint64_t k;
									for(k = 0; k < matrix.size()-1; k++)
										(matrix.at(k)).push_back(((*it).rowID.at(k))[j]);
									matrix.at(k).push_back(joinResults[1][i]);
									break;
								}
							}
						}
						else if( disCount1[joinResults[0][i]] != 0 )
						{
							int count = 0;
							uint64_t **unJoinResults = new uint64_t *[2];
							unJoinResults[0] = new uint64_t[disCount1[joinResults[0][i]]];
							unJoinResults[1] = new uint64_t[disCount1[joinResults[0][i]]];

							for(uint64_t u=0; u < resultSize; u++)
							{
								if(joinResults[0][u] == joinResults[0][i])
								{
									int faFlag = 0;
									for(int h = 0; h < count; h++)
										if(joinResults[1][u] == unJoinResults[1][h])
										{
											faFlag = 1;
											break;
										}

									if(faFlag == 0 )
									{
										unJoinResults[0][count] = joinResults[0][u];
										unJoinResults[1][count] = joinResults[1][u];
										count++;
									}
								}
							}

							for(uint64_t u = 0; u < (*it).size; u++)
							{
								if( joinResults[0][i] == (*it2)[u] )
								{
									for(uint64_t k = 0; k < count; k++)
									{
										uint64_t n;
										for(n = 0; n < matrix.size()-1; n++)
											(matrix.at(n)).push_back(((*it).rowID.at(n))[u]);
										matrix.at(n).push_back(unJoinResults[1][k]);
									}
								}
							}

							free(unJoinResults[0]);
							free(unJoinResults[1]);
							free(unJoinResults);

							disCount1[joinResults[0][i]]=0;							// Mark as 0 to not add dupes
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
					//cout << "newsize is " << newSize << endl;
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
				if( (*it1) == relationID1 )
				{

					std::vector< std::vector<uint64_t> > matrix;						// Create a 2d array
					matrix.resize((*it).relationID.size()+1);							// with as many columns as tempresult struct plus one to fit new column


					unordered_map<uint64_t,uint64_t> disCount;							// Unordered map to count appearances of each rowid in joinresults
					for(uint64_t i = 0; i < resultSize; i++)
						disCount[joinResults[1][i]]++;


					for(uint64_t i = 0; i < resultSize; i++)
						if( disCount[joinResults[1][i]] == 1 )
						{
							for(uint64_t j = 0; j < (*it).size; j++ )
							{
								if( joinResults[1][i] == (*it2)[j] )
								{

									uint64_t k;
									for(k = 0; k < matrix.size()-1; k++)
										(matrix.at(k)).push_back(((*it).rowID.at(k))[j]);
									matrix.at(k).push_back(joinResults[0][i]);
									break;
								}
							}
						}
						else if( disCount[joinResults[1][i]] != 0 )
						{

							std::vector< std::vector<uint64_t> > uniqueR;
							for(uint64_t j = 0; j < (*it).size; j++ )
							{
								if( joinResults[1][i] == (*it2)[j] )
								{
									std::vector< uint64_t > tempv;
									for(uint64_t k = 0; k < matrix.size()-1; k++)
										tempv.push_back((((*it).rowID.at(k))[j]));
									tempv.push_back(joinResults[0][i]);

									int exflag = 0;

									for(uint64_t k = 0; k < uniqueR.size(); k++)
										if(uniqueR.at(k) == tempv )
										{
											exflag = 1;
											break;
										}

									if( exflag == 1)	continue;
									uniqueR.push_back(tempv);
								}
							}

							uint64_t times = disCount[joinResults[1][i]]/uniqueR.size();

							for(uint64_t j = 0; j < times; j++)
							{
								for(uint64_t k = 0; k < uniqueR.size(); k++)
								{
										for(uint64_t o = 0; o < matrix.size(); o++)
										{
											matrix.at(o).push_back((uniqueR.at(k)).at(o));
										}
								}
							}
							disCount[joinResults[1][i]] = 0;							// Mark as 0 to not add dupes
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
					//cout << "newsize is " << newSize << endl;
																						// TODO: cleanup the memory mess
																						// before doing this:
					(*it).rowID = newRowID;
					return;																// Just once
				}
		}

	}
	else cout << "mistakes\n";

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
			if((*check)->rel_views == tr->relationID.at(j)) //relid = check id.get results now
			{
				for(i = 0 ; i < tr->size ; i++)
				{
					uint64_t* temp = tr->rowID.at(j);
					row = temp[i];
					relID = tr->relationID.at(j);

					//cout << "number added is " << (ra->relations.at(relID))->relation[(*check)->rel_cols][row] << endl;
					checksum += (ra->relations.at(relID))->relation[(*check)->rel_cols][row];
				}
				// break;
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




// void jointest()
// {
// 	cout << "ok" << std::endl;

// 	uint64_t * rowID1 = new uint64_t[3];
// 	uint64_t * rowID2 = new uint64_t[5];

// 	rowID1[0] = 1;
// 	rowID1[1] = 2;
// 	rowID1[2] = 3;

// 	rowID2[0] = 1;
// 	rowID2[1] = 2;
// 	rowID2[2] = 3;
// 	rowID2[3] = 4;
// 	rowID2[4] = 5;

// 	uint64_t *arr1 = new uint64_t[3];
// 	uint64_t *arr2 = new uint64_t[5];

// 	arr1[0] = 5;
// 	arr1[1] = 1;
// 	arr1[2] = 9;

// 	arr2[0] = 7;
// 	arr2[1] = 6;
// 	arr2[2] = 1;
// 	arr2[3] = 9;
// 	arr2[4] = 1;

// 	Table_Info *tableInfo1 = init_table_info(rowID1, arr1, 3);
// 	Table_Info *tableInfo2 = init_table_info(rowID2, arr2, 5);

// 	daIndex **indx = DAIndexArrayCreate(tableInfo2->bck_array);

// 	for(int i=0; i<16; i++)
// 	{
// 		cout << "bucket" << i << endl;
// 		for(int j=0;j<indx[i]->bucket->size;j++)
// 			cout << indx[i]->bucket->table[j] << endl;
// 		cout << endl;
// 	}

// 	result *res = getResults(tableInfo2,tableInfo1,indx);

// 	print_results(res);

// }

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
