#include "helper_functions.h"
#define N 4

Table_Info* init_table_info(int* a, int* b, int size)		// Initializes the variables and structs of table info, a=keys, b=payloads
{
	Table_Info* ti = malloc(sizeof(Table_Info));
	int32_t LSB;

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
	int32_t* pSumDsp = calloc(ti->histSize,sizeof(int32_t));


	int32_t mask = (1 << N) - 1;								// Creating histogram, tha mporouse kalista na einai lathos

	for(int i = 0; i < ti->histSize; i++)
	{
		LSB = ti->tuples_table[i]->payload & mask;
		ti->histogram[LSB]++; 
	}

	
	ti->pSum[0] = 0;											// Creating pSum
	
	for(int i = 1; i < ti->histSize; i++)
	{
		ti->pSum[i] = ti->pSum[i-1] + ti->histogram[i-1];
		pSumDsp[i] = pSumDsp[i-1] + ti->histogram[i-1]; 
	}


	int prg = 0;											// Filling buckets. To conversion apo panou se giwrgou den einai etoimo
	bucket *bck;

	int32_t* R_Payload = calloc(size,sizeof(int32_t));
	int32_t* R_Id = calloc(size,sizeof(int32_t));
	
	for(int i = 0 ; i < size ; i++){
		LSB = ti->tuples_table[i]->payload & mask;
		R_Payload[pSumDsp[LSB]] = ti->tuples_table[i]->payload;
		R_Id[pSumDsp[LSB]] = ti->tuples_table[i]->key;

		pSumDsp[LSB]++;
	}

	bucket_array *A = ti->bck_array;

	A = malloc(sizeof(bucket_array));
	A->size = ti->histSize;

	A->bucketArray = malloc(sizeof(bucket*) * (ti->histSize));

	for(int i =0; i < ti->histSize; i++)
		A->bucketArray[i] = malloc(sizeof(bucket));

	for (int i = 0 ; i < ti->histSize ; i++)//check periptwsh opou to bucket den iparxei
	{
		bck = A->bucketArray[i];
		bck->size = ti->histogram[i];
		if(ti->histogram[i] != 0){
			//printf("i is %d and hist[i] is %d \n",i,histogram_A[i]);
			bck->tuplesArray = (tuple**)malloc(sizeof(tuple*) * ti->histogram[i]);
			for(int j = 0 ; j < ti->histogram[i] ; j++){
				bck->tuplesArray[j] = malloc(sizeof(tuple));
				bck->tuplesArray[j]->key = R_Id[prg];
				bck->tuplesArray[j]->payload = R_Payload[prg];
				//printf("tuple reordered with tuple->id %d and tuple->payload %d \n",IdA[prgA],Payload_A[prgA]);
				printf("A reordered->Id %d\n",bck->tuplesArray[j]->key);
				prg++;
			}
		}
	}

	return ti;
}
