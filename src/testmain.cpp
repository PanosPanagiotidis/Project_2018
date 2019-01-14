#include <iostream>

#include "../header/relation_loader.h"
#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/randarr.h"
#include "../header/includes.h"
#include <unistd.h>
#include <ctime>

int main(void)
{

	relationArray *rArray = init_relations();

	sleep(1);
	std::clock_t start;
	double duration;
	start = std::clock();
	threadpool* tp = threadpool_init(NUM_THREADS);

	while(1){
		queryBatch *qBatch = QueryInput();									// Read & Store a batch of queries
		if (qBatch == NULL) break;
		tempResults *tra;

		std::vector<Relations*> originals;
		std::vector<uint64_t> rels;
		std::vector<Query*>::iterator q;
		for(q = qBatch->queries.begin(); q != qBatch->queries.end() ; q++)
		{
			relationArray *localTempArray = createTempRelArray(rArray,(*q));
			tra = queryExecute((*q),localTempArray,tp);							// Execute each query in batch

			getChecksum(&(tra->res.at(0)),localTempArray,(*q)->checksums);
			deleteTR(&tra);
			delete(tra);
			deleteRelations(&localTempArray);
		}

		deleteQuery(&qBatch);
	}
	duration = (std::clock() - start)/(double) CLOCKS_PER_SEC;
	cout << "Total Duration " << duration << endl;
	deleteRelations(&rArray);


	return 0;
}
