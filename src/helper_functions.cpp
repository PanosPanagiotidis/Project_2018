#include "../header/thread_scheduler.h"
#include <iostream>
// #define N 10
using namespace std;




Table_Info* init_table_info(uint64_t* a, uint64_t* b, int size,threadpool* THREAD_POOL)		// Initializes the variables and structs of table info, a=keys, b=payloads
{
	// extern threadpool* THREAD_POOL;
	//Mask the least significant bits.Payload & mask = LSB


	Table_Info* ti = new Table_Info;

	if(ti == NULL){
		fprintf(stderr,"Error allocating space for Table Info with size %d \n",size);
		exit(0);
	}

	ti->rows = size;


	ti->histSize = 1 << N; //16



	ti->pSum = new uint64_t[ti->histSize];	//prefix sum for each bucket.i.e starting at 0,3,5 items from reordered array
	for(int i = 0 ; i < ti->histSize ; i++)
		ti->pSum[i] = 0;

	if(ti->pSum == NULL){
		fprintf(stderr,"Error allocating space for pSum table\n");
		exit(0);
	}

	ti->pSumDsp = new uint64_t[ti->histSize]; //Displacement pSum.
	for(int i = 0 ; i < ti->histSize ; i++)
		ti->pSumDsp[i] = 0;

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

	histArg** arg_table;
	arg_table = new histArg*[jobs];
	for(int i = 0 ; i < jobs ; i++){
		arg_table[i] = new histArg;
	}

	for(int i = 0 ;i < jobs ; i++){
		row_from+=chunk;
		row_to+=chunk;
		if(i == jobs-1 && leftovers > 0)
			row_to+=leftovers;

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






	ti->pSum[0] = 0;											// Creating pSum
	for(int i = 1; i < ti->histSize; i++)
	{
		ti->pSum[i] = ti->pSum[i-1] + ti->histogram[i-1];
		ti->pSumDsp[i] = ti->pSumDsp[i-1] + ti->histogram[i-1];
	}


	int prg = 0;											// Filling buckets.

	ti->R_Payload = new uint64_t[size];

	if(ti->R_Payload == NULL){
		fprintf(stderr,"Error allocating space for Reordered Payload Table\n");
		exit(0);
	}

	ti->R_Id = new uint64_t[size];

	if(ti->R_Id == NULL){
		fprintf(stderr,"Error allocating space for Reordered Id Table\n");
		exit(0);
	}

	hashArg** harg_table = new hashArg*[jobs];
	row_from = 0;
	row_from -=chunk;
	row_to = 0;

	uint64_t** global_dsp = new uint64_t*[jobs];

	for(int i = 0 ; i < jobs ; i++){
		global_dsp[i] = new uint64_t[ti->histSize];
		for(int j = 0; j < ti->histSize ;j++){
			if(i==0)
				global_dsp[i][j] = ti->pSum[j];
			else
				global_dsp[i][j] = 0;
		}
	}

	for(int i = 1 ; i < jobs ;i++){
		for(int j = 0 ; j < ti->histSize;j++){
			global_dsp[i][j] += hists[i-1][j] +global_dsp[i-1][j];
		}
	}

	for(int i = 0 ;i < jobs ; i++){
		

		row_from+=chunk;
		row_to+=chunk;
		if(i == jobs -1 && leftovers > 0)
			row_to+=leftovers;

		harg_table[i] = new hashArg;
		harg_table[i]->payloads = b;
		harg_table[i]->rowId = a;
		harg_table[i]->fromRow = row_from;
		harg_table[i]->toRow = row_to;
		harg_table[i]->loc = i;
		harg_table[i]->dsp = global_dsp[i];
		harg_table[i]->stored_row = ti->R_Id;
		harg_table[i]->stored_payloads = ti->R_Payload;



		add_work(THREAD_POOL->Q,&partitionJob,harg_table[i]);
	}

	thread_wait();

	for(int i = 0 ; i < jobs ;i++){
		delete[] global_dsp[i];
		delete[] hists[i];
		delete harg_table[i];
		delete arg_table[i];
	}
	delete[] global_dsp;
	delete[] hists;
	delete[] harg_table;
	delete[] arg_table;


	ti->bck_array = new bucket_array;

	if(ti->bck_array == NULL){
		fprintf(stderr,"Error allocating space for a Bucket Array\n");
		exit(0);
	}

	ti->bck_array->size = ti->histSize;

	ti->bck_array->bck = new bucket*[ti->histSize];

	if(ti->bck_array->bck == NULL){
		fprintf(stderr,"Error allocating space for size*bucketArray\n");
		exit(0);
	}

	for(int i =0; i < ti->histSize; i++){
		ti->bck_array->bck[i] = new bucket;
		if(ti->bck_array->bck[i] == NULL){
			fprintf(stderr,"Error allocating space for bucket\n");
			exit(0);
		}
	}

	for (int i = 0 ; i < ti->histSize ; i++)
	{
		ti->bck_array->bck[i]->size = ti->histogram[i];

		if(ti->histogram[i] != 0){ // Bucket is not existant


			ti->bck_array->bck[i]->tuplesArray = new toumble*[ti->histogram[i]];

			for(int j = 0 ; j < ti->histogram[i] ; j++){

				ti->bck_array->bck[i]->tuplesArray[j] = new toumble;

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
	uint64_t i,j;

	for(i = 0 ; i < (*ti)->histSize ; i++){//iterate and free items

			for(j = 0 ; j < (*ti)->histogram[i] ; j++){

				if((*ti)->bck_array->bck[i]->tuplesArray != NULL){

					delete ((*ti)->bck_array->bck[i]->tuplesArray[j]);
				}

			}
				delete[] ((*ti)->bck_array->bck[i]->tuplesArray);
				delete ((*ti)->bck_array->bck[i]);
	}

	// for(i = 0 ; i < (*ti)->rows ; i++){
	// 	free((*ti)->tuples_table[i]);
	// }


	delete[] ((*ti)->bck_array->bck);
	delete ((*ti)->bck_array);
	delete[] ((*ti)->histogram);
	// free((*ti)->tuples_table);
	delete[] ((*ti)->R_Payload);
	delete[] ((*ti)->R_Id);
	delete[] ((*ti)->pSumDsp);
	delete[] ((*ti)->pSum);
	delete ((*ti));

}