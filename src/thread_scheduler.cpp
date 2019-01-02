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
	tp->Q = jobq_init();


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

		void *(*func_buff)(void*);
		void* args;
		Job* task = getJob();

		if(task){
			func_buff = task->function;
			args = task->arg;
			free(task);
		}

		pthread_mutex_unlock(&tp->access);

		func_buff(args);

	}
	return NULL;
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

	pthread_cond_broadcast(&tp->hasjobs);
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

	pthread_mutex_unlock(&tp->access);
}