#include "helper_functions.h"
#define N 4

Table_Info* init_table_info(int* a, int* b, int size)		// Initializes the variables and structs of table info, a=keys, b=payloads
{
	Table_Info* ti = malloc(sizeof(Table_Info));

	ti->tuples_table= malloc(sizeof(tuple*)*size);			// Creating tuple array (rowID,value)
	for(int i = 0; i < size; i++)
	{
		ti->tuples_table[i] = malloc(sizeof(tuple));
		ti->tuples_table[i]->key = a[i];
		ti->tuples_table[i]->payload = b[i];
	}

	ti->histSize = 1 << N;
	ti->histogram= calloc(ti->histSize,sizeof(int32_t));
	ti->pSum = calloc(ti->histSize,sizeof(int32_t));
	ti->bck_array = malloc(sizeof(bucket_array));

	for(int i =0; i < ti->histSize; i++)
		ti->bck_array[i] = malloc(sizeof(bucket));

	int mask = (1 << N) - 1;								// Creating histogram, tha mporouse kalista na einai lathos

	for(int i = 0; i < ti->histSize; i++)
	{
		int off = ti->tuples_table[i]->payload && mask;
		ti->histogram[off]++; 
	}

	int sum = 0;											// Creating pSum

	for(int i = 1; i < ti->histSize; i++)
	{
		sum += ti->histogram[i-1];
		ti->pSum[i] = sum; 
	}

	int prg = 0;											// Filling buckets. To conversion apo panou se giwrgou den einai etoimo

	for (i = 0 ; i < ti->histSize ; i++)//check periptwsh opou to bucket den iparxei
	{
		bck = ti->bck_array[i];
		bck->size = histogram_A[i];
		if(ti->histogram[i] != 0){
			//printf("i is %d and hist[i] is %d \n",i,histogram_A[i]);
			bck->tuplesArray = (tuple**)malloc(sizeof(tuple*) * ti->histogram[i]);
			for(j = 0 ; j < ti->histogram[i] ; j++){
				bck->tuplesArray[j] = malloc(sizeof(tuple));
				bck->tuplesArray[j]->key = R_A_Id[prgA];
				bck->tuplesArray[j]->payload = R_A_Payload[prgA];
				//printf("tuple reordered with tuple->id %d and tuple->payload %d \n",IdA[prgA],Payload_A[prgA]);
				printf("A reordered->Id %d\n",bck->tuplesArray[j]->key);
				prg++;
			}
		}
	}

	return ti;
}
