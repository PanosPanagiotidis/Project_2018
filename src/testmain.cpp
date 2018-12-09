#include <iostream>

#include "../header/relation_loader.h"
#include "../header/parser.h"
#include "../header/relationops.h"
#include "../header/randarr.h"

int main(void)
{

/*

	relationArray *rArray = init_relations();											// Read & Store the relations
*/
	vector<uint64_t> rowdID1 = create_column(10,1);
	vector<uint64_t> rowdID2 = create_column(20,1);

	vector<uint64_t> payload1 = create_column(10,0);
	vector<uint64_t> payload2 = create_column(20,0);

	Relations *r1 = new Relations;
	Relations *r2 = new Relations;

	r1->relation = new uint64_t*[1];
	for(int i = 0 ; i < 5 ; i++){
		r1->relation[i] = new uint64_t[5];
	}

	r2->relation = new uint64_t*[1];
	for(int i = 0 ; i < 5 ; i++){
		r2->relation[i] = new uint64_t[5];
	}

	r1->size = 5;
	r2->size = 5;

	r1->numColumns = 1;
	r2->numColumns = 1;

	r1->relation[0][0] = 1000;
	r1->relation[0][4] = 1212321;
	r1->relation[0][1] = 12;
	r1->relation[0][2] = 5921;
	r1->relation[0][3] = 9417;

	r2->relation[0][0] = 521321;
	r2->relation[0][4] = 92;
	r2->relation[0][1] = 151;
	r2->relation[0][2] = 432;
	r2->relation[0][3] = 9842;

	relationArray *ra = new relationArray;
	ra->relations.push_back(r1);
	ra->relations.push_back(r2);


	queryBatch *qBatch = QueryInput();									// Read & Store a batch of queries
	tempResultArray *tra = new tempResultArray;


	
	std:vector<Query*>::iterator q;
	for(q = qBatch->queries.begin(); q != qBatch->queries.end() ; q++){
	//for(int i=0; i< qBatch->queryCount; i++)
		tra = queryExecute((*q),ra);								// Execute each query in batch
		getChecksum(tra,ra,(*q)->checksums);
	}



	if(tra == NULL){
		cout << "OOF" << endl;
	}
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