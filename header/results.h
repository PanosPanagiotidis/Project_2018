#ifndef __RESULTS_H__
#define __RESULTS_H__

#include "../header/thread_scheduler.h"
#include "../header/structs.h"
#include "../header/daindex.h"
#include <vector>
#include "../header/includes.h"

using namespace std;

typedef struct result{
	std::vector<toumble*> results_array;
}result;



result* getResults(Table_Info* ,Table_Info* ,daIndex** ,threadpool* );

void destroy_results(result** );

void print_results(result* );


#endif