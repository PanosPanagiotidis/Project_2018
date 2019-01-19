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


	uint64_t mask = (1 << N) - 1;

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
		delete args[i];
	}

	delete[] args;

	rlist* temp;
	for(int i = 0 ; i < jobs ; i++){
		while(partials[i] != NULL){
			for(int j = 0 ; j < partials[i]->size ;j++){
				toumble* temp = new toumble;
				temp->key = partials[i]->ts[j].key;
				temp->payload = partials[i]->ts[j].payload;
				temp->rids = partials[i]->ts[j].rids;
				r->results_array.push_back(temp);
			}
			temp = partials[i];
			partials[i] = partials[i]->next;
			delete[] temp->ts;
			delete temp;
		}
	}

	// for(int i = 0 ; i < jobs ; i++){
	// 	while(olds[i] != NULL){
	// 		for(int j = 0 ;j < olds[i]->size;j++){
	// 			toumble *t = new toumble;
	// 			t->key=olds[i]->ts[j].key;
	// 			old->results_array.push_back(t);
	// 		}
	// 		temp = olds[i];
	// 		olds[i] = olds[i]->next;
	// 		delete[] temp->ts;
	// 		delete temp;
	// 	}
	// }

	// delete[] partials;
	// delete[] olds;



	result** rets = new result*[2];
	rets[0] = r;
	// rets[1] = old;

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
	delete (*r);

}