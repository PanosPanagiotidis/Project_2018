#include <stdlib.h>

#include "../header/parser.h"
#include "../header/relationops.h"

void queryExecute(Query *qr, relationArray *relArray)
{
	Query * orderedQuery = queryReorder(qr);											// Reorders predicates in query for optimization

	// tempResults setup;

	for( int i=0; i < orderedQuery->total_p; i++ )										// Each predicate is either a join or a filter
		if( orderedQuery->p[i].type == JOIN )
		{
			// res = join(stuff,tempResults);
			// tempResults update;
		}
		else
		{
			// res = filter(stuff,tempResults);
			// tempResults update;
		}

	// checkSum view;

	free(orderedQuery);

	return 	/* results */;
}




Query *queryReorder(Query *qr)
{
	Query * newQr = malloc(sizeof(Query));

	newQr->total_p = qr->total_p;
	newQr->checksums = qr->checksums;

	newQr->p = qr->p;																	// Currently changes nothing

	return newQr;
}