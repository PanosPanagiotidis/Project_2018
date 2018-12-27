#include "stdio.h"
#include "results.h"
#include "daindex.h"
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

#define N 10


/*
*	Iterates through column of non Indexed Array nonIndexed for matches on
*	Indexed Array T.
*	Results are stored in a tuple with tuple.key being the nonIndexed columns Id
*	and tuple.payload being the Indexed columns Id
*/

result* getResults(Table_Info *T,Table_Info* nonIndexed,daIndex **Index){

	int key,payload;

	result *r = new result;

	if(r == NULL){
		fprintf(stderr,"Error allocating space for result struct \n");
		exit(0);
	}


	// uint64_t result_size = (1024*1024)/sizeof(toumble);
	uint64_t counter = 0;

	// r->results_array = (toumble*)malloc(sizeof(toumble)*result_size);
	// r->next = NULL;
	// r->size = 0;

	uint64_t r_key;
	uint64_t LSB;
	uint64_t hash2_value;
	uint64_t chain_pos;

	uint64_t mask = (1 << N) - 1;


	result *head = r;

	for(int i = 0 ; i < nonIndexed->rows ;i++){
		payload = nonIndexed->R_Payload[i];
		key = nonIndexed->R_Id[i];


		LSB = payload & mask; // Least Significant Bytes kept to go directly to the bucket in question

		hash2_value = payload % HASHFUNC_RANGE;//Position in bucket array to find results

		chain_pos = Index[LSB]->bucket->table[hash2_value];

		if(chain_pos == 0) continue;

		while(chain_pos != 0)
		{
			r_key = chain_pos - 1;		//item in chain

			if(T->bck_array->bck[LSB]->tuplesArray[r_key]->payload == payload)
			{
				toumble* t = new toumble;
				t->key = key;
				t->payload = T->bck_array->bck[LSB]->tuplesArray[r_key]->key;
				r->results_array.push_back(t);

				// if(counter == result_size-1)
				// 	{
				// 		counter = 0;
				// 		result *temp = (result*)malloc(sizeof(result));

				// 		if(temp == NULL){
				// 			fprintf(stderr,"Error allocating space for result struct\n");
				// 			exit(0);
				// 		}

				// 		temp->results_array = (toumble*)malloc(sizeof(toumble)*result_size);

				// 		if(temp->results_array == NULL){
				// 			fprintf(stderr,"Error allocating space for results array\n");
				// 			exit(0);
				// 		}

				// 		temp->size = 0;
				// 		temp->next = NULL;
				// 		r->next = temp;
				// 		r = temp;
				// 	}

				// r->results_array[counter].key = key; //non Indexed Column key/rowId
				// r->results_array[counter].payload = T->bck_array->bck[LSB]->tuplesArray[r_key]->key;	//Indexed Column key/rowId

				// counter++;
				// r->size++;
			}

			chain_pos = Index[LSB]->chain->array[chain_pos];


		}

	}
	return r;
}

/*
*	Prints kinda prettily the results
*
*/

void print_results(result* r){


	printf("\n");
	printf("|	Table A Row Id  	|	Table B Row Id 		|\n");

	for(int i = 0 ; i < r->results_array.size() ; i++ ){

			std::cout << "|	 	"<< r->results_array.at(i)->key <<"		|	 	"<< r->results_array.at(i)->payload << "		|\n";

	}
	printf("\n");
}



void destroy_results(result** r){

	for(int i = 0 ; i < (*r)->results_array.size();i++){
		delete((*r)->results_array.at(i));
	}

	vector<toumble*>().swap((*r)->results_array);

}