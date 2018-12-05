#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"
#include "../header/helper_functions.h"
#include "../header/daindex.h"
#include "../header/results.h"

#include <iostream>

void queryExecute(Query *qr, relationArray *relArray)
{
	Query *orderedQuery = queryReorder(qr);												// Reorders predicates in query for optimization purposes

	// tempResults setup
	// resultArray setup

	std::vector<predicates*>::iterator it;

	for( it = orderedQuery->p.begin(); it != orderedQuery->p.end(); it++)
		if( (*it)->type == JOIN)														// Each predicate is either a join or a filter
		{
			// join
			// resultArray update
		}
		else
		{
			// filter
			// resultArray update
		}

	// checksum view

	delete orderedQuery;

	return /*results*/;

}


Query *queryReorder(Query *qr)
{
	Query *newQr = new Query;

	newQr->p = qr->p;
	newQr->relations = qr->relations;
	newQr->checksums = qr->checksums;

	return newQr;
}

void relation_filter(predicates *pred, relationArray *rArray/*, tempResults*/)			// TODO: Create a result struct and make it return that
{

	int relationId = pred->relation1;
	int columnId = pred->column1;
	uint64_t filter = pred->filter;

	Relations * currentRelation = rArray->relations.at(relationId);


	// TODO: Check if relation in tempresults and if it is, assign that to currentRelation
	// else, proceed as follows


	std::vector<int> results;

	int rID = 0;
	for(uint64_t row=0; row < currentRelation->size; row++, rID++)
	{



		switch (pred->type) {
			case EQ_FILTER:
				if( filter == currentRelation->relation[columnId][rID])
					results.push_back(rID);
				break;

			case GT_FILTER:
				if( filter <= currentRelation->relation[columnId][rID])
					results.push_back(rID);
				break;

			case LT_FILTER:
				if( filter >= currentRelation->relation[columnId][rID])
					results.push_back(rID);
				break;

			default:
				//This should never happen
				break;
		}
	}


	// TODO: Update tempResult

	return ; // TODO: results

}


void relation_join(predicates *pred, relationArray *rArray)
{
	int relationId1 = pred->relation1;
	int relationId2 = pred->relation2;
	int columnId1   = pred->column1;
	int columnId2   = pred->column2;

	Relations * currentRelation1 = rArray->relations.at(relationId1);
	Relations * currentRelation2 = rArray->relations.at(relationId2);

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

}


uint64_t *createRowID(uint64_t rSize)
{
	uint64_t *rowID = new uint64_t[rSize];
	for(uint64_t i=0; i < rSize; i++)	rowID[i] = i;
	return rowID;
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

	result *res = getResults(tableInfo1,tableInfo2,indx);

	print_results(res);

}






