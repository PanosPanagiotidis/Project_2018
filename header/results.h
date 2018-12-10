#ifndef __RESULTS_H__
#define __RESULTS_H__

#include "structs.h"
#include "daindex.h"
#include <vector>


typedef struct result{
	std::vector<toumble*> results_array;
}result;


result* getResults(Table_Info* ,Table_Info* ,daIndex** );

void destroy_results(result** );

void print_results(result* );


#endif