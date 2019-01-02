#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>



typedef struct Job{
	struct Job* prev;
	void (*function)(void* arg);
	void* arg;
}Job;

typedef struct Job_Q{
	pthread_mutex_t access;
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
	pthread_mutex_t count_lock;
	pthread_cond_t all_idle;
	int num_threads;
	Job_Q *Q;
}threapool;

threadpool* threadpool_init(int );
Job_Q* jobq_init(void );
void* thread_work(void* );