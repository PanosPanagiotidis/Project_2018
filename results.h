#ifndef __RESULTS_H__
#define __RESULTS_H__

#include "structs.h"
#include "daindex.h"


typedef struct result{
	tuple *results_array;
	struct result *next;
	int size;
}result;

result* getResults(Table_Info* ,Table_Info* ,daIndex** );


#endif