#include "../header/thread_scheduler.h"
#include <iostream>

using namespace std;

void* print_thread_info(void* arg){
	cout << "hi there " << endl;
}

int main(void){
	threadpool* tp = threadpool_init(3);
	for(int i = 0 ; i < 50 ; i++){
		add_work(tp->Q,(print_thread_info),NULL);
	}
	for(int i = 0 ; i < 3 ; i++){
		pthread_join(tp->threads[i]->thread,NULL);
	}
}