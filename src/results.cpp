#include "stdio.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "../header/results.h"

using namespace std;




/*
*	Iterates through column of non Indexed Array nonIndexed for matches on
*	Indexed Array T.
*	Results are stored in a tuple with tuple.key being the nonIndexed columns Id
*	and tuple.payload being the Indexed columns Id
*/

result** getResults(Table_Info *T,Table_Info* nonIndexed,daIndex **Index,threadpool* tp,int flag){

	int key,payload;
	int jobs = 1<<N;

	result *r = new result;
	result *old = new result;

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

	// cout << "psum " << endl;
	// for(int i = 0 ; i < jobs ; i++){
	// 	cout << nonIndexed->pSum[i]<<endl;
	// }

	// cout << "-------------"<<endl;
	rlist** partials = new rlist*[jobs];
	rlist** olds = new rlist*[jobs];
	for(int i = 0 ; i < jobs ; i++){
		partials[i] = new rlist;
		olds[i] = new rlist;
	}

	joinArg** args = new joinArg*[jobs];


	for(int i = 0 ; i < jobs ; i++){
		args[i] = new joinArg;

		args[i]->indexed = T;
		args[i]->nonIndexed= nonIndexed;
		args[i]->Index = Index;
		args[i]->partials=partials[i];
		args[i]->bucket = i;
		args[i]->olds = olds[i];
		args[i]->flag = flag;
		add_work(tp->Q,&joinJob,args[i]);
	}


	thread_wait();

	
	for(int i = 0 ; i < jobs ; i++){
		while(partials[i] != NULL){
			for(int j = 0 ; j < partials[i]->size ;j++){
				toumble* temp = new toumble;
				temp->key = partials[i]->ts[j].key;
				temp->payload = partials[i]->ts[j].payload;
				r->results_array.push_back(temp);
			}
			partials[i] = partials[i]->next;
		}
	}

	for(int i = 0 ; i < jobs ; i++){
		while(olds[i] != NULL){
			for(int j = 0 ;j < olds[i]->size;j++){
				toumble *t = new toumble;
				t->key=olds[i]->ts[j].key;
				old->results_array.push_back(t);
			}
			olds[i] = olds[i]->next;
		}
	}

	
	// vector<toumble*>::iterator t1;
	
	// for(t1 = partials.begin(); t1 != partials.end(); t1++){
	// 	r->results_array.push_back(*t1);
	// }


	//result *head = r;

	// for(int i = 0 ; i < nonIndexed->rows ;i++){
	// 	payload = nonIndexed->R_Payload[i];
	// 	key = nonIndexed->R_Id[i];


	// 	LSB = payload & mask; // Least Significant Bytes kept to go directly to the bucket in question

	// 	hash2_value = payload % HASHFUNC_RANGE;//Position in bucket array to find results

	// 	chain_pos = Index[LSB]->bucket->table[hash2_value];

	// 	if(chain_pos == 0) continue;

	// 	while(chain_pos != 0)
	// 	{
	// 		r_key = chain_pos - 1;		//item in chain

	// 		if(T->bck_array->bck[LSB]->tuplesArray[r_key]->payload == payload)
	// 		{
	// 			toumble* t = new toumble;
	// 			t->key = key;
	// 			t->payload = T->bck_array->bck[LSB]->tuplesArray[r_key]->key;
	// 			r->results_array.push_back(t);

	// 			// if(counter == result_size-1)
	// 			// 	{
	// 			// 		counter = 0;
	// 			// 		result *temp = (result*)malloc(sizeof(result));

	// 			// 		if(temp == NULL){
	// 			// 			fprintf(stderr,"Error allocating space for result struct\n");
	// 			// 			exit(0);
	// 			// 		}

	// 			// 		temp->results_array = (toumble*)malloc(sizeof(toumble)*result_size);

	// 			// 		if(temp->results_array == NULL){
	// 			// 			fprintf(stderr,"Error allocating space for results array\n");
	// 			// 			exit(0);
	// 			// 		}

	// 			// 		temp->size = 0;
	// 			// 		temp->next = NULL;
	// 			// 		r->next = temp;
	// 			// 		r = temp;
	// 			// 	}

	// 			// r->results_array[counter].key = key; //non Indexed Column key/rowId
	// 			// r->results_array[counter].payload = T->bck_array->bck[LSB]->tuplesArray[r_key]->key;	//Indexed Column key/rowId

	// 			// counter++;
	// 			// r->size++;
	// 		}

	// 		chain_pos = Index[LSB]->chain->array[chain_pos];


	// 	}

	// }
	result** rets = new result*[2];
	rets[0] = r;
	rets[1] = old;

	return rets;
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