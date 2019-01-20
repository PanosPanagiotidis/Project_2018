#ifndef __RESULTS_H__
#define __RESULTS_H__

#include "thread_scheduler.h"
#include "structs.h"
#include "daindex.h"
#include "includes.h"
#include <vector>

using namespace std;

typedef struct result{
	std::vector<toumble*> results_array;
}result;



result** getResults(Table_Info* ,Table_Info* ,daIndex** ,threadpool* ,int );

void destroy_results(result** );

void print_results(result* );


#endif