#ifndef __DA_INDEX_H__
#define __DA_INDEX_H__

#include <stdint.h>
#include "structs.h"

#define HASH_RANGE 101

typedef struct bucketHashTableData
{
	int position;								// Position of start of chain in chainArray
	int32_t unhashedData;						// Copy of unhashed key for collision handling
} bucketHashTableData;

typedef struct chainArray {
	int size;
	int *array;
} chainArray;

typedef struct daIndex
{
	bucketHashTableData bucket[HASH_RANGE];
	chainArray *chain;
} daIndex;


int bucketHashFunction(int32_t);
daIndex **createDAIndexArray(bucket_array *);
daIndex *createDAIndex(bucket *);

#endif