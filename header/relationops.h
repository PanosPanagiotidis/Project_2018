#ifndef __RELATIONOPS_H__
#define __RELATIONOPS_H__

#include "../header/parser.h"
#include "../header/relation_loader.h"

typedef struct tempResultArray{
	std::vector<uint64_t *> rowID;
	std::vector<int> relationID;														// IDs of relations in tempResultArray
	uint64_t size;																		// Number of rowids in each vector element
}tempResultArray;

typedef struct tempResults{
	std::vector<tempResultArray> res;
}tempResults;


void queryExecute(Query *, relationArray *);											// TODO: Should return results

Query *queryReorder(Query *);															// Reorders predicates in a query

uint64_t *createRowID(uint64_t);

void relation_join(predicates *, relationArray *);

uint64_t *tempResultsLookup(tempResults *,int, uint64_t *);

void jointest(void);

#endif