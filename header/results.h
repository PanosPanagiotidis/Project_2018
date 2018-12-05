#ifdef __cplusplus
extern "C" {
#endif


#ifndef __RESULTS_H__
#define __RESULTS_H__

#include "structs.h"
#include "daindex.h"


typedef struct result{
	toumble *results_array;
	struct result *next;
	int size;
}result;


result* getResults(Table_Info* ,Table_Info* ,daIndex** );

void destroy_results(result** );

void print_results(result* );

#endif

#ifdef __cplusplus
}
#endif