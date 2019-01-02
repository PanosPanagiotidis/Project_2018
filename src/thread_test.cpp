#include "../header/thread_scheduler.h"
#include <iostream>


int main(void){
	threadpool* tp = threadpool_init(3);
	for(int i = 0 ; i < 3 ; i++){
		pthread_join(tp->threads[i]->thread,NULL);
	}
}