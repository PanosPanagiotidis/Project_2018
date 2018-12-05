#ifdef __cplusplus
extern "C" {
#endif


#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

Table_Info* init_table_info(uint64_t* , uint64_t* , int );
void Destroy_Table_Data(Table_Info** );

#endif

#ifdef __cplusplus
}
#endif