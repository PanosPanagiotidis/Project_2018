#include "helper_functions.h"
#define N 4

Table_Info* init_table_info(int* a, int* b, int size)		// Initializes the variables and structs of table info, a=keys, b=payloads
{
	Table_Info* ti = malloc(sizeof(Table_Info));

	if(ti == NULL){
		fprintf(stderr,"Error allocating space for Table Info with size %d \n",size);
		exit(0);
	}

	int32_t LSB;

	ti->tuples_table= malloc(sizeof(tuple*)*size);			// Creating tuple array (rowID,value)

	if(ti->tuples_table == NULL){
		fprintf(stderr,"Error allocating space for Tuples Table\n");
		exit(0);
	}

	for(int i = 0; i < size; i++)
	{
		ti->tuples_table[i] = malloc(sizeof(tuple));

		if(ti->tuples_table[i] == NULL){
			fprintf(stderr,"Error allocating space for tuple\n");
			exit(0);
		}

		ti->tuples_table[i]->key = a[i];

		ti->tuples_table[i]->payload = b[i];
	}

	ti->histSize = 1 << N;
	ti->histogram= calloc(ti->histSize,sizeof(int32_t));

	if(ti->histogram == NULL){
		fprintf(stderr,"Error allocating space for histogram\n");
		exit(0);
	}	

	ti->pSum = calloc(ti->histSize,sizeof(int32_t));

	if(ti->pSum == NULL){
		fprintf(stderr,"Error allocating space for pSum table\n");
		exit(0);
	}

	int32_t* pSumDsp = calloc(ti->histSize,sizeof(int32_t));

	if(pSumDsp == NULL){
		fprintf(stderr,"Error allocating space for pSumDsp\n");
		exit(0);
	}	

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

	if(R_Payload == NULL){
		fprintf(stderr,"Error allocating space for Reordered Payload Table\n");
		exit(0);
	}	

	int32_t* R_Id = calloc(size,sizeof(int32_t));

	if(R_Id == NULL){
		fprintf(stderr,"Error allocating space for Reordered Id Table\n");
		exit(0);
	}	
	
	for(int i = 0 ; i < size ; i++){
		LSB = ti->tuples_table[i]->payload & mask;
		R_Payload[pSumDsp[LSB]] = ti->tuples_table[i]->payload;
		R_Id[pSumDsp[LSB]] = ti->tuples_table[i]->key;

		pSumDsp[LSB]++;
	}

	bucket_array *A = ti->bck_array;

	A = malloc(sizeof(bucket_array));

	if(A == NULL){
		fprintf(stderr,"Error allocating space for a Bucket Array\n");
		exit(0);
	}	

	A->size = ti->histSize;

	A->bucketArray = malloc(sizeof(bucket*) * (ti->histSize));

	if(A->bucketArray == NULL){
		fprintf(stderr,"Error allocating space for size*bucketArray\n");
		exit(0);
	}	

	for(int i =0; i < ti->histSize; i++){
		A->bucketArray[i] = malloc(sizeof(bucket));
		if(A->bucketArray[i] == NULL){
			fprintf(stderr,"Error allocating space for bucket\n");
			exit(0);
		}	
	}

	for (int i = 0 ; i < ti->histSize ; i++)
	{
		bck = A->bucketArray[i];
		bck->size = ti->histogram[i];

		if(ti->histogram[i] != 0){ // Bucket is not existant

			bck->tuplesArray = (tuple**)malloc(sizeof(tuple*) * ti->histogram[i]);

			for(int j = 0 ; j < ti->histogram[i] ; j++){
				bck->tuplesArray[j] = malloc(sizeof(tuple));
				if(bck->tuplesArray[j] == NULL){
					fprintf(stderr,"Error allocating space for tuple\n");
					exit(0);
				}	

				bck->tuplesArray[j]->key = R_Id[prg];

				bck->tuplesArray[j]->payload = R_Payload[prg];

				prg++;//offset for bucket
			}
		}
	}

	return ti;
}


void Destroy_Table_Data(Table_Info* ti){ //TODO
	int i,j;

	for(i = 0 ; i < ti->histSize ; i++){
		for(j = 0 ; j < ti->histogram[i] ; j++){
			free(ti->bucketArray[i]->tuplesArray[i]);
		}
		free(ti->bucketArray[i]);
	}


	free(ti->tuples_table);
	free(R_Payload);
	free(R_Id);
	free(pSumDsp);
	free(pSum);

}