#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"

void queryExecute(Query *qr, relationArray *relArray)
{
	Query *orderedQuery = queryReorder(qr);												// Reorders predicates in query for optimization purposes

	// tempResults setup

	std::vector<predicates*>::iterator it;

	for( it = orderedQuery->p.begin(); it != orderedQuery->p.end(); it++)
		if( (*it)->type == JOIN)														// Each predicate is either a join or a filter
		{
			// join
			// tempResults update
		}
		else
		{
			// filter
			// tempResults update
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