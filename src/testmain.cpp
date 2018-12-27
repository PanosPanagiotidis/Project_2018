#include <iostream>

#include "../header/relation_loader.h"
#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/randarr.h"
#include <unistd.h>

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
	while(1){
		queryBatch *qBatch = QueryInput();									// Read & Store a batch of queries
		if (qBatch == NULL) break;
		tempResults *tra;

		std::vector<Relations*> originals;
		std::vector<uint64_t> rels;
		std:vector<Query*>::iterator q;
		for(q = qBatch->queries.begin(); q != qBatch->queries.end() ; q++){
		//for(int i=0; i< qBatch->queryCount; i++)
			tra = queryExecute((*q),rArray,originals,rels);							// Execute each query in batch
			getChecksum(&(tra->res.at(0)),rArray,(*q)->checksums);
			deleteTR(&tra);
			delete(tra);
			replace_filtered(rArray,originals,rels);

			vector<Relations*>().swap(originals);
			vector<uint64_t>().swap(rels);
		}

		deleteQuery(&qBatch);
	}

	deleteRelations(&rArray);

/*
	std::vector<Relations *>::iterator it;

	for(it = rArray->relations.begin(); it != rArray->relations.end(); it++)
	{
		uint64_t **rt = (*it)->relation;
		for(uint64_t i=0; i<(*it)->numColumns; i++, cout << endl)
			for(uint64_t j=0; j<(*it)->size; j++ )
			{
				cout << rt[i][j] << " ";
			}
	}

	relation_join(NULL,NULL);
*/
	//jointest();

	return 0;
}

// 0 1|0.1>2000|1.1
// F