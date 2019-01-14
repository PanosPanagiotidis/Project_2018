#include "../header/thread_scheduler.h"
#include <iostream>
// #define N 10
using namespace std;




Table_Info* init_table_info(uint64_t* a, uint64_t* b, int size,threadpool* THREAD_POOL)		// Initializes the variables and structs of table info, a=keys, b=payloads
{
	// extern threadpool* THREAD_POOL;
	uint64_t LSB;
	uint64_t mask = (1 << N) - 1; //Mask the least significant bits.Payload & mask = LSB


	Table_Info* ti = (Table_Info*)malloc(sizeof(Table_Info));

	if(ti == NULL){
		fprintf(stderr,"Error allocating space for Table Info with size %d \n",size);
		exit(0);
	}

	ti->rows = size;
	// ti->tuples_table= (toumble**)malloc(sizeof(toumble*)*size);			// Creating tuple array (rowID,value)

	// if(ti->tuples_table == NULL){
	// 	fprintf(stderr,"Error allocating space for Tuples Table\n");
	// 	exit(0);
	// }

	// for(int i = 0; i < size; i++)	//creating a tuples array for each combo of rowId + payload
	// {
	// 	ti->tuples_table[i] = (toumble*)malloc(sizeof(toumble));

	// 	if(ti->tuples_table[i] == NULL){
	// 		fprintf(stderr,"Error allocating space for tuple\n");
	// 		exit(0);
	// 	}

	// 	ti->tuples_table[i]->key = a[i];

	// 	ti->tuples_table[i]->payload = b[i];
	// }

	ti->histSize = 1 << N; //16
	ti->histogram= (uint64_t*)calloc(ti->histSize,sizeof(uint64_t));

	if(ti->histogram == NULL){
		fprintf(stderr,"Error allocating space for histogram\n");
		exit(0);
	}

	ti->pSum = (uint64_t*)calloc(ti->histSize,sizeof(uint64_t));	//prefix sum for each bucket.i.e starting at 0,3,5 items from reordered array

	if(ti->pSum == NULL){
		fprintf(stderr,"Error allocating space for pSum table\n");
		exit(0);
	}

	ti->pSumDsp = (uint64_t*)calloc(ti->histSize,sizeof(uint64_t)); //Displacement pSum.

	if(ti->pSumDsp == NULL){
		fprintf(stderr,"Error allocating space for pSumDsp\n");
		exit(0);
	}

	//split the table to threads
	uint64_t row_from = 0;
	uint64_t row_to = 0;
	int jobs = 0;
	int chunk = size/NUM_THREADS;
	int leftovers = size%NUM_THREADS;
	row_from-=chunk;
	jobs = NUM_THREADS;



	if(size < NUM_THREADS){
		jobs = size;
	}
	uint64_t** hists = new uint64_t*[jobs];

	for(int i = 0 ; i < jobs ; i++)
		hists[i] = new uint64_t[ti->histSize];


	histArg** arg_table;
	arg_table = new histArg*[jobs];

	for(int i = 0 ;i < jobs ; i++){
		row_from+=chunk;
		row_to+=chunk;
		if(i == jobs-1 && leftovers > 0)
			row_to+=leftovers;
		arg_table[i] = new histArg;
		arg_table[i]->payloads = b;
		arg_table[i]->rowId = a;

		arg_table[i]->fromRow = row_from;
		arg_table[i]->toRow = row_to;


		arg_table[i]->thread_hists = hists;
		arg_table[i]->loc = i;

		add_work(THREAD_POOL->Q,&histogramJob,arg_table[i]);

	}



	thread_wait();

	ti->histogram = rebuild_hist(hists,jobs);

	for(int i = 0 ; i < jobs ;i++){
		delete[] hists[i];
		delete arg_table[i];
	}

	delete[] arg_table;


	// for(int i = 0; i < ti->rows; i++)
	// {
	// 	LSB = b[i] & mask;
	// 	ti->histogram[LSB]++;
	// }

	//cout << "This is a new table " << endl;

	ti->pSum[0] = 0;											// Creating pSum

	for(int i = 1; i < ti->histSize; i++)
	{
		ti->pSum[i] = ti->pSum[i-1] + ti->histogram[i-1];
		// cout << "ti->pSum is " << ti->pSum[i] << endl;
		ti->pSumDsp[i] = ti->pSumDsp[i-1] + ti->histogram[i-1];
	}


	int prg = 0;											// Filling buckets.

	ti->R_Payload = (uint64_t*)calloc(size,sizeof(uint64_t));

	if(ti->R_Payload == NULL){
		fprintf(stderr,"Error allocating space for Reordered Payload Table\n");
		exit(0);
	}

	ti->R_Id = (uint64_t*)calloc(size,sizeof(uint64_t));

	if(ti->R_Id == NULL){
		fprintf(stderr,"Error allocating space for Reordered Id Table\n");
		exit(0);
	}

	hashArg** harg_table = new hashArg*[jobs];
	row_from = 0;
	row_from -=chunk;
	row_to = 0;

	for(int i = 0 ;i < jobs ; i++){

		row_from+=chunk;
		row_to+=chunk;
		if(i == jobs -1 && leftovers > 0)
			row_to+=leftovers;

		harg_table[i] = new hashArg;
		harg_table[i]->payloads = b;
		harg_table[i]->rowId = a;
		harg_table[i]->stored_payloads = ti->R_Payload;
		harg_table[i]->stored_rows = ti->R_Id;
		harg_table[i]->fromRow = row_from;
		harg_table[i]->toRow = row_to;
		harg_table[i]->dsp = ti->pSumDsp;
		harg_table[i]->loc = i;

		add_work(THREAD_POOL->Q,&partitionJob,harg_table[i]);
	}


	// for(int i = 0 ; i < size ; i++){ //Reordering of Id and Payload arrays.Afterwards stored in new arrays
	// 	LSB = b[i] & mask;

	// 	ti->R_Payload[ti->pSumDsp[LSB]] = b[i];

	// 	ti->R_Id[ti->pSumDsp[LSB]] = a[i];

	// 	ti->pSumDsp[LSB]++;
	// }

	thread_wait();


	bucket_array *A = ti->bck_array;

	ti->bck_array = (bucket_array*)malloc(sizeof(bucket_array));

	if(ti->bck_array == NULL){
		fprintf(stderr,"Error allocating space for a Bucket Array\n");
		exit(0);
	}

	ti->bck_array->size = ti->histSize;

	ti->bck_array->bck = (bucket**)malloc(sizeof(bucket*) * (ti->histSize));

	if(ti->bck_array->bck == NULL){
		fprintf(stderr,"Error allocating space for size*bucketArray\n");
		exit(0);
	}

	for(int i =0; i < ti->histSize; i++){
		ti->bck_array->bck[i] = (bucket*)malloc(sizeof(bucket));
		if(ti->bck_array->bck[i] == NULL){
			fprintf(stderr,"Error allocating space for bucket\n");
			exit(0);
		}
	}

	for (int i = 0 ; i < ti->histSize ; i++)
	{
		ti->bck_array->bck[i]->size = ti->histogram[i];

		if(ti->histogram[i] != 0){ // Bucket is not existant

			ti->bck_array->bck[i]->tuplesArray = (toumble**)malloc(sizeof(toumble*) * ti->histogram[i]);

			for(int j = 0 ; j < ti->histogram[i] ; j++){

				ti->bck_array->bck[i]->tuplesArray[j] = (toumble*)malloc(sizeof(toumble));

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


void Destroy_Table_Data(Table_Info** ti){
	int i,j;
	bucket* bk;
	bucket_array *A = (*ti)->bck_array;


	for(i = 0 ; i < (*ti)->histSize ; i++){//iterate and free items

			for(j = 0 ; j < (*ti)->histogram[i] ; j++){

				if((*ti)->bck_array->bck[i]->tuplesArray != NULL){

					free((*ti)->bck_array->bck[i]->tuplesArray[j]);
				}

			}
				free((*ti)->bck_array->bck[i]->tuplesArray);
				free((*ti)->bck_array->bck[i]);
	}

	// for(i = 0 ; i < (*ti)->rows ; i++){
	// 	free((*ti)->tuples_table[i]);
	// }


	free((*ti)->bck_array->bck);
	free((*ti)->bck_array);
	free((*ti)->histogram);
	// free((*ti)->tuples_table);
	free((*ti)->R_Payload);
	free((*ti)->R_Id);
	free((*ti)->pSumDsp);
	free((*ti)->pSum);
	free((*ti));
}