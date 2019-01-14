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



	//relationArray *rArray = init_relations();											// Read & Store the relations
/*
	vector<uint64_t> rowdID1 = create_column(10,1);
	vector<uint64_t> rowdID2 = create_column(20,1);
	vector<uint64_t> rowdID3 = create_column(20,1);

	vector<uint64_t> payload1 = create_column(10,0);
	vector<uint64_t> payload2 = create_column(20,0);
	vector<uint64_t> payload3 = create_column(20,0);

	Relations *r1 = new Relations;
	Relations *r2 = new Relations;
	Relations *r3 = new Relations;

	r1->relation = new uint64_t*[1];
	for(int i = 0 ; i < 5 ; i++){
		r1->relation[i] = new uint64_t[7];
	}

	r2->relation = new uint64_t*[1];
	for(int i = 0 ; i < 5 ; i++){
		r2->relation[i] = new uint64_t[5];
	}

	r3->relation = new uint64_t*[1];
	for(int i = 0 ; i < 5 ; i++){
		r3->relation[i] = new uint64_t[5];
	}


	r1->size = 8;
	r2->size = 5;
	r3->size = 5;

	r1->numColumns = 1;
	r2->numColumns = 1;
	r3->numColumns = 1;


	r1->relation[0][0] = 1;
	r1->relation[0][1] = 11;
	r1->relation[0][2] = 12;
	r1->relation[0][3] = 14;
	r1->relation[0][4] = 12;
	r1->relation[0][5] = 12;
	r1->relation[0][6] = 12;

	r2->relation[0][0] = 1;
	r2->relation[0][1] = 8;
	r2->relation[0][2] = 12;
	r2->relation[0][3] = 1;
	r2->relation[0][4] = 10;

	r3->relation[0][0] = 1;
	r3->relation[0][1] = 12;
	r3->relation[0][2] = 1;
	r3->relation[0][3] = 1;
	r3->relation[0][4] = 1;

	relationArray *ra = new relationArray;
	ra->relations.push_back(r1);
	ra->relations.push_back(r2);
	ra->relations.push_back(r3);


*/

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
			// deleteTR(&tra);
			// delete(tra);
			deleteRelations(&localTempArray);
		}

		deleteQuery(&qBatch);
	}
	duration = (std::clock() - start)/(double) CLOCKS_PER_SEC;
	cout << "Total Duration " << duration << endl;
	deleteRelations(&rArray);


	return 0;
}
