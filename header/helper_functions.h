#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include "thread_scheduler.h"
#include "includes.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

Table_Info* init_table_info(uint64_t* , uint64_t* , int ,threadpool* );
void Destroy_Table_Data(Table_Info** );


Table_Info* init_table_info2(uint64_t** a, uint64_t* b,int relNum, int relID, int size,threadpool* THREAD_POOL);		// Initializes the variables and structs of table info, a=keys, b=payloads

#endif
