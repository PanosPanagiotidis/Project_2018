#ifndef THREAD_SCHEDULER_H
#define THREAD_SCHEDULER_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>
#include "../header/structs.h"
#include "../header/daindex.h"
#include "../header/includes.h"
#include <vector>

using namespace std;

typedef struct rlist{
	toumble* ts;
	struct rlist* next;
	uint64_t size;
}rlist;


typedef struct histArg{
	uint64_t* payloads;
	uint64_t* rowId;
	uint64_t fromRow;
	uint64_t toRow;
	uint64_t** thread_hists;
	int loc;
}histArg;

typedef struct hashArg{
	uint64_t* stored_payloads;
	uint64_t* stored_rows;
	uint64_t* histogram;
	uint64_t* rowId;
	uint64_t* payloads;
	uint64_t fromRow;
	uint64_t toRow;
	uint64_t* dsp;
	int loc;
}hashArg;


typedef struct Job{
	struct Job* prev;
	void* (*function)(void* );
	void* arg;
}Job;

typedef struct Job_Q{
	Job* first;
	Job* last;
	int len;
}Job_Q;

typedef struct thread_info{
	long int id;
	pthread_t thread;
	struct threadpool *pool;
}thread_info;


typedef struct threadpool{
	thread_info** threads;
	int working;
	int alive;
	pthread_mutex_t access;
	pthread_mutex_t dsp;
	pthread_cond_t all_idle;
	pthread_cond_t hasjobs;
	int num_threads;
	Job_Q *Q;
}threadpool;

typedef struct joinArg{
	Table_Info* indexed;
	Table_Info* nonIndexed;
	daIndex** Index;
	int bucket;
	rlist* partials;
	rlist* olds;
	int flag;

}joinArg;

threadpool* threadpool_init(int );
Job_Q* jobq_init(void );
void* thread_work(void* );
void thread_wait(void );
void* histogramJob(void* );
void* partitionJob(void* );
void* joinJob(void* );
// void* print_thread_info(void *);
int add_work(Job_Q* ,void *(*function_p)(void*),void* );
Job* getJob();

uint64_t* rebuild_hist(uint64_t** ,uint64_t );

#endif