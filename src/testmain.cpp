#include <iostream>

#include "../header/relation_loader.h"
#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/includes.h"
#include <unistd.h>
#include <ctime>

int main(void)
{

	relationArray *rArray = init_relations();

	sleep(1);
	struct timespec start, finish;
	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);
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
	destroy_pool(tp);

	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout << "Total Duration " << elapsed << endl;
	deleteRelations(&rArray);

	exit(0);
	return 0;
}
