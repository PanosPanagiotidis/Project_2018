#include "../header/thread_scheduler.h"
#include <iostream>

using namespace std;

pthread_mutex_t w;

threadpool* tp;

threadpool* threadpool_init(int num_threads){
	pthread_mutex_init(&w,NULL);
	tp = new threadpool;
	if (tp == NULL){
		cout << "error on threadpool init.No memory to allocate" << endl;
		exit(1);
	}

	tp->threads = new thread_info*[num_threads];
	tp->num_threads = num_threads;

	tp->working = 0;
	
	//init job q here
	Job_Q* q = NULL;
	q = jobq_init();


	pthread_mutex_init(&(tp->count_lock),NULL);
	pthread_cond_init(&(tp->all_idle),NULL);

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
	pthread_mutex_init(&(q->access),NULL);

	return q;

}

void threadpool_wait(threadpool* tp){

}


void* thread_work(void* arg){
	pthread_mutex_lock(&tp->count_lock);
	tp->working++;
	pthread_mutex_unlock(&tp->count_lock);
	cout << "thread with TID " << pthread_self() << endl;
	
	pthread_mutex_lock(&tp->count_lock);
	tp->working++;
	pthread_mutex_unlock(&tp->count_lock);

	return NULL;
}
