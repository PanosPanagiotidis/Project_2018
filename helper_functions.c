#include "helper_functions.h"
#define N 4

Table_Info* init_table_info(int* a, int* b, int size)		// Initializes the variables and structs of table info, a=keys, b=payloads
{
	int32_t LSB;
	int32_t mask = (1 << N) - 1;


	Table_Info* ti = malloc(sizeof(Table_Info));

	if(ti == NULL){
		fprintf(stderr,"Error allocating space for Table Info with size %d \n",size);
		exit(0);
	}

	ti->rows = size;
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

	ti->histSize = 1 << N; //16
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

	ti->pSumDsp = calloc(ti->histSize,sizeof(int32_t));

	if(ti->pSumDsp == NULL){
		fprintf(stderr,"Error allocating space for pSumDsp\n");
		exit(0);
	}	


	for(int i = 0; i < ti->rows; i++)
	{
		LSB = ti->tuples_table[i]->payload & mask;
		ti->histogram[LSB]++; 
	}


	for(int i = 0 ; i < (1 << N) ; i++){
		printf("ti->histogram[i] %d\n",ti->histogram[i]);
	}

	ti->pSum[0] = 0;											// Creating pSum
	
	for(int i = 1; i < ti->histSize; i++)
	{
		ti->pSum[i] = ti->pSum[i-1] + ti->histogram[i-1];
		ti->pSumDsp[i] = ti->pSumDsp[i-1] + ti->histogram[i-1]; 
	}


	int prg = 0;											// Filling buckets. To conversion apo panou se giwrgou den einai etoimo
	bucket *bk;

	ti->R_Payload = calloc(size,sizeof(int32_t));

	if(ti->R_Payload == NULL){
		fprintf(stderr,"Error allocating space for Reordered Payload Table\n");
		exit(0);
	}	

	ti->R_Id = calloc(size,sizeof(int32_t));

	if(ti->R_Id == NULL){
		fprintf(stderr,"Error allocating space for Reordered Id Table\n");
		exit(0);
	}	
	
	for(int i = 0 ; i < size ; i++){
		LSB = ti->tuples_table[i]->payload & mask;

		ti->R_Payload[ti->pSumDsp[LSB]] = ti->tuples_table[i]->payload;

		ti->R_Id[ti->pSumDsp[LSB]] = ti->tuples_table[i]->key;

		ti->pSumDsp[LSB]++;
	}

	bucket_array *A = ti->bck_array;

	ti->bck_array = malloc(sizeof(bucket_array));

	if(ti->bck_array == NULL){
		fprintf(stderr,"Error allocating space for a Bucket Array\n");
		exit(0);
	}	

	ti->bck_array->size = ti->histSize;

	ti->bck_array->bck = malloc(sizeof(bucket*) * (ti->histSize));

	if(ti->bck_array->bck == NULL){
		fprintf(stderr,"Error allocating space for size*bucketArray\n");
		exit(0);
	}	

	for(int i =0; i < ti->histSize; i++){
		ti->bck_array->bck[i] = malloc(sizeof(bucket));
		if(ti->bck_array->bck[i] == NULL){
			fprintf(stderr,"Error allocating space for bucket\n");
			exit(0);
		}	
	}

	for (int i = 0 ; i < ti->histSize ; i++)
	{
		//bk = A->bck[i];
		ti->bck_array->bck[i]->size = ti->histogram[i];

		if(ti->histogram[i] != 0){ // Bucket is not existant

			ti->bck_array->bck[i]->tuplesArray = (tuple**)malloc(sizeof(tuple*) * ti->histogram[i]);

			for(int j = 0 ; j < ti->histogram[i] ; j++){

				ti->bck_array->bck[i]->tuplesArray[j] = malloc(sizeof(tuple));

				if(ti->bck_array->bck[i]->tuplesArray[j] == NULL){
					fprintf(stderr,"Error allocating space for tuple\n");
					exit(0);
				}	

				ti->bck_array->bck[i]->tuplesArray[j]->key = ti->R_Id[prg];

				ti->bck_array->bck[i]->tuplesArray[j]->payload = ti->R_Payload[prg];

				prg++;//offset for bucket
			}
		}else{
			ti->bck_array->bck[i]->tuplesArray = NULL;
		}
	}

	return ti;
}


void Destroy_Table_Data(Table_Info** ti){ //TODO
	int i,j;
	bucket* bk;
	bucket_array *A = (*ti)->bck_array;


	for(i = 0 ; i < (*ti)->histSize ; i++){

		// if((*ti)->histogram[i] != 0){

			for(j = 0 ; j < (*ti)->histogram[i] ; j++){

				if((*ti)->bck_array->bck[i]->tuplesArray != NULL){

					free((*ti)->bck_array->bck[i]->tuplesArray[j]);
				}

			}
				free((*ti)->bck_array->bck[i]->tuplesArray);
				free((*ti)->bck_array->bck[i]);
	//	}
	}

	for(i = 0 ; i < (*ti)->rows ; i++){
		free((*ti)->tuples_table[i]);
	}


	free((*ti)->bck_array->bck);
	free((*ti)->bck_array);
	free((*ti)->histogram);
	free((*ti)->tuples_table);
	free((*ti)->R_Payload);
	free((*ti)->R_Id);
	free((*ti)->pSumDsp);
	free((*ti)->pSum);
	free((*ti));

}