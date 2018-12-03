#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/relation_loader.h"

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