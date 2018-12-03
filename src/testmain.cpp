#include <iostream>

#include "../header/relation_loader.h"
#include "../header/parser.h"
#include "../header/relationops.h"

int main(void)
{


	relationArray *rArray = init_relations();											// Read & Store the relations

	queryBatch *qBatch = QueryInput();												// Read & Store a batch of queries
	/*
	for(int i=0; i< qBatch->queryCount; i++)
		queryExecute(qBatch->queries[i],rArray);										// Execute each query in batch

	// TODO: Possibly add  loop to request input of another query batch?

	// TODO: Present results

	// TODO: Free structs

	*/

	std::vector<Relations *>::iterator it;

	for(it = rArray->relations.begin(); it != rArray->relations.end(); it++)
	{
		cout << "lel" << endl;
	}

	return 0;
}