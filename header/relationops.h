#ifndef __RELATIONOPS_H__
#define __RELATIONOPS_H__

#include "../header/parser.h"
#include "../header/structs.h"



void queryExecute(Query *, relationArray *);											// TODO: Should return results

Query *queryReorder(Query *);															// Reorders predicates in a query

#endif