#include "stdio.h"
#include "results.h"
#include "daindex.h"
#include <stdlib.h>

#define N 4


result* getResults(Table_Info *T,Table_Info* nonIndexed,daIndex **Index){

	int key,payload;

	result *r = malloc(sizeof(result));
	

	int32_t result_size = (1024*1024)/sizeof(tuple);
	int32_t counter = 0;

	r->results_array = malloc(sizeof(tuple)*result_size);
	r->next = NULL;
	r->size = 0;

	int32_t r_key;
	int32_t LSB;
	int32_t hash2_value;
	int32_t chain_pos;

	int32_t mask = (1 << N) - 1;


	//for (int i = 0 ; i < 50 ; i++) printf("row is %d\n",nonIndexed->R_Payload[i]);
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

			if(T->R_Payload[r_key] == payload)
			{

				if(counter == result_size-1)
					{
						counter = 0;
						result *temp = malloc(sizeof(result));
						temp->results_array = malloc(sizeof(tuple)*result_size);
						temp->size = 0;
						temp->next = NULL;
						r->next = temp;
						r = temp;
					}

				r->results_array[counter].key = key; //non Indexed Column key/rowId
				r->results_array[counter].payload = T->R_Id[r_key];	//Indexed Column key/rowId

				//valto to key sta results se morfh tuple (key,key);
				counter++;
				r->size++;
			}

			chain_pos = Index[LSB]->chain->array[chain_pos];


		}

	}
	return head;

}

void print_results(result* r){
	int loop = r->size;
	int i = 0;

	printf("\n");
	printf("|	Table A Row Id  	|	Table B Row Id 		|\n");	

	while(loop >= 0 ){

		if((loop == 0) && (r->next != NULL)){

			r = r->next;
			loop = r->size;
			i = 0;

		}else if((loop == 0) && (r->next == NULL)){

			loop--;

		}else{
			
			printf("|	 	%d		|	 	%d		|\n",r->results_array[i].key,r->results_array[i].payload);
			loop--;
			i++;

		}
	}
	printf("\n"); 
}

void destroy_results(result** r){
	result *temp;


	while((*r) != NULL){
		free((*r)->results_array);
		temp = (*r);
		(*r) = (*r)->next;
		free(temp);
	}

}