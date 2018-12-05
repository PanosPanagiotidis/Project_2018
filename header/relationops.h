#ifndef __RELATIONOPS_H__
#define __RELATIONOPS_H__

#include "../header/parser.h"
#include "../header/relation_loader.h"



void queryExecute(Query *, relationArray *);											// TODO: Should return results

Query *queryReorder(Query *);															// Reorders predicates in a query

uint64_t *createRowID(uint64_t);

void relation_join(predicates *, relationArray *);


void jointest(void);

#endif