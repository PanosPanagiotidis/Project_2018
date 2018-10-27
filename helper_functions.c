#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "structs.h"

#endif
#include <stdlib.h>


bucket* create_bucket(int size){
    bucket *arr = malloc(sizeof(bucket));
    arr->tuplesArray = malloc(sizeof(tuple) * size);//allocate space for array of buckets
    return arr;
}
