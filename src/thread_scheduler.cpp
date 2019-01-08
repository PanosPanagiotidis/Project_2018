#include "../header/thread_scheduler.h"
#include <iostream>


using namespace std;

// #define N 10

pthread_mutex_t w = PTHREAD_MUTEX_INITIALIZER;


threadpool* tp;

threadpool* threadpool_init(int num_threads){
	tp = new threadpool;
	if (tp == NULL){
		cout << "error on threadpool init.No memory to allocate" << endl;
		exit(1);
	}

	tp->threads = new thread_info*[num_threads];
	tp->num_threads = num_threads;

	tp->working = 0;
	tp->alive = 0;
	//init job q here
	tp->Q = jobq_init();


	pthread_mutex_init(&(tp->dsp),NULL);
	pthread_mutex_init(&(tp->access),NULL);
	pthread_cond_init(&(tp->all_idle),NULL);
	pthread_cond_init(&(tp->hasjobs),NULL);
	for(int i = 0 ; i < num_threads ; i++){
		tp->threads[i] = new thread_info;
		tp->threads[i]->pool = tp;
		pthread_create(&(tp->threads[i]->thread),NULL,&thread_work,NULL);
		tp->threads[i]->id = i;

	}

	return tp;
}

Job_Q* jobq_init(void){
	Job_Q *q;
	q = new Job_Q;
	if(q == NULL){
		cout << "error allocating space for job q" << endl;
		exit(1);
	}

	q->len = 0;
	q->first = NULL;
	q->last = NULL;

	return q;

}

Job* getJob(){

	Job* task = tp->Q->first;
	switch(tp->Q->len){
		case 0:
			break;//no jobs
		case 1:
			tp->Q->first = NULL;
			tp->Q->last = NULL;
			tp->Q->len = 0;
			break;
		default:
			tp->Q->first = task->prev;
			tp->Q->len--;
	}


	return task;
}

void* thread_work(void* arg){

	while(1){

		pthread_mutex_lock(&tp->access);
		while(tp->Q->len == 0){
			pthread_cond_wait(&tp->hasjobs,&tp->access);
		}

		tp->working++;


		void *(*func_buff)(void*);
		void* args;
		Job* task = getJob();

		pthread_mutex_unlock(&tp->access);
		if(task){
			func_buff = task->function;
			args = task->arg;
			free(task);
		}

		
		func_buff(args);

		pthread_mutex_lock(&tp->access);

		tp->working--;
		if(tp->working == 0){
			pthread_cond_signal(&tp->all_idle);
		}

		pthread_mutex_unlock(&tp->access);
	}


	return NULL;
}

void* histogramJob(void* arg){

	uint64_t size = 1 << N;
	uint64_t mask = (1<<N) -1;
	uint64_t LSB;
	histArg *params = static_cast<histArg*>(arg);

	uint64_t* histogram = new uint64_t[size];
	for(int i = 0 ; i < size ; i++)
		histogram[i] = 0;

	for(int i = params->fromRow ; i < params->toRow ;i++){
		LSB = params->payloads[i] & mask;
		histogram[LSB]++;
	}

	params->thread_hists[params->loc] = histogram;

	return NULL;
}


void* partitionJob(void* arg){
	uint64_t mask = (1<<N) -1;
	hashArg *params = static_cast<hashArg*>(arg);
	uint64_t LSB;
	uint64_t pl;
	uint64_t loc;
	for(int i = params->fromRow ; i < params->toRow ; i++){
		pthread_mutex_lock(&w);
		pl = params->payloads[i];
		LSB = pl & mask;
		loc = params->dsp[LSB];
		params->dsp[LSB]++;
		pthread_mutex_unlock(&w);

		
		params->stored_payloads[loc] = pl;

		params->stored_rows[loc] = params->rowId[i];

		
	}
		//	pthread_mutex_unlock(&w);
	return NULL;
}

void* joinJob(void* arg){
	joinArg* params = static_cast<joinArg*>(arg);
	uint64_t mask = (1<<N)-1;
	uint64_t max = 1<<N;
	uint64_t LSB;
	int bck = params->bucket;
	int row_to;
	int row_from;

	row_from = params->nonIndexed->pSum[bck];

	if(bck == max-1)
		row_to = params->nonIndexed->rows;
	else
		row_to = params->nonIndexed->pSum[bck+1];

	// cout << "bck is " << bck <<"max is " << max << endl;
	// cout << "row_from " << row_from << endl;
	// cout << "row_to " << row_to << endl;
	uint64_t* payloads = params->nonIndexed->R_Payload;
	uint64_t* rows = params->nonIndexed->R_Id;
	daIndex** index = params->Index;
	rlist* results = params->partials;
	uint64_t rsize = (128*1000)/sizeof(toumble);

	// results = new rlist;
	results->next = NULL;
	results->size = 0;
	results->ts = new toumble[rsize];
	//keep head here
	rlist *head = results;
	uint64_t value;
	uint64_t key;

	uint64_t hash2;
	uint64_t chain_pos;
	uint64_t rkey;
	//std::vector<toumble*> locals;
	for(int i = row_from ; i < row_to ;i++){

		value = payloads[i];
		key = rows[i];

		LSB = value & mask;
		hash2 = value % HASHFUNC_RANGE;
		chain_pos = index[LSB]->bucket->table[hash2];

		if(chain_pos == 0) continue;

		while(chain_pos != 0){

			rkey = chain_pos-1;
			if(params->indexed->bck_array->bck[LSB]->tuplesArray[rkey]->payload == value){

				if(results->size == rsize){
					results->next = new rlist;
					results = results->next;
					results->next = NULL;
					results->ts = new toumble[rsize];
					results->size = 0;
				}
				
				results->ts[results->size].key = key;
				results->ts[results->size].payload = params->indexed->bck_array->bck[LSB]->tuplesArray[rkey]->key;
				results->size++;

			}

			chain_pos = params->Index[LSB]->chain->array[chain_pos];
		}
	}

	params->partials = head;

	return NULL;

}

void thread_wait(){
	pthread_mutex_lock(&tp->access);
	while(tp->Q->len > 0 || tp->working > 0){
		pthread_cond_wait(&tp->all_idle,&tp->access);
	}
	pthread_mutex_unlock(&tp->access);
}

// void* print_thread_info(void* arg){
// 	cout << "hi there " << endl;
// 	return NULL;
// }

int add_work(Job_Q* q,void *(*function_p)(void*),void* args){
	Job* newJob;
	newJob = new Job;

	newJob->function = function_p;
	newJob->arg = args;

	pthread_mutex_lock(&tp->access);

	newJob->prev = NULL;

	switch(q->len){
		case 0:
			q->first = newJob;
			q->last = newJob;
			break;

		default:
		q->last->prev=newJob;
		q->last = newJob;

	}

	q->len++;

	pthread_cond_broadcast(&tp->hasjobs);
	pthread_mutex_unlock(&tp->access);
}

uint64_t* rebuild_hist(uint64_t** thread_hists,uint64_t histsize){
	uint64_t size = 1<<N;
	uint64_t* histogram = new uint64_t[size];

	for(int i = 0 ; i < size ;i++)
		histogram[i] = 0;

	for(int i = 0 ; i < histsize ; i++){
		// histogram[i] = 0;
		for(int j = 0 ; j < size ; j++){
			histogram[j] += thread_hists[i][j];
		}

	}
	return histogram;
}