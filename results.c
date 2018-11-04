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
	//int32_t chain_pos;

	int32_t mask = (1 << N) - 1;

	// if((*Index) == NULL){
	// 	printf("is null\n");
	// }


	//for (int i = 0 ; i < 50 ; i++) printf("row is %d\n",nonIndexed->R_Payload[i]);
	result *head = r;

	for(int i = 0 ; i < nonIndexed->rows ;i++){
		payload = nonIndexed->R_Payload[i];
		key = nonIndexed->R_Id[i];


		LSB = payload & mask; // Least Significant Bytes kept to go directly to the bucket in question

		hash2_value = payload % HASHFUNC_RANGE;//Position in bucket array to find results

		int chain_pos = Index[LSB]->bucket->table[hash2_value];

		// if (chain_pos == 0) printf("Data is nowhere to be found in the other table");

		while(chain_pos != 0)
		{
			r_key = chain_pos - 1;		//item in chain

			// printf("rpayload %d payload %d\n",T->R_Payload[r_key],payload);
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

				r->results_array[counter].key = key;
				r->results_array[counter].payload = T->R_Id[r_key];
				//printf("key is %d and R_Key is %d\n",r->results_array[counter].key,r->results_array[counter].payload);

				//valto to key sta results se morfh tuple (key,key);
				counter++;
				r->size++;
			}

			chain_pos = Index[LSB]->chain->array[chain_pos];

		}

	}
	return head;

}