#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include "../header/thread_scheduler.h"
#include "../header/includes.h"
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

Table_Info* init_table_info(uint64_t* , uint64_t* , int ,threadpool* );
void Destroy_Table_Data(Table_Info** );

#endif
