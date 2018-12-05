#ifdef __cplusplus
extern "C" {
#endif

#ifndef __DA_INDEX_H__
#define __DA_INDEX_H__

#include "structs.h"
#include <stdint.h>

#define HASHFUNC_RANGE 11

/*

typedef struct bucketHashTableData														// Hash Table Entry
{
	int position;																		// Position of start of chain in chainArray
	uint64_t unhashedData;																// Copy of unhashed data for collision handling
} bucketHashTableData;

*/

typedef struct bucketHashTable
{
	uint64_t *table;																			// Position of start of chain in chainArray
	int size;
	int occupiedCount;
} bucketHashTable;


typedef struct chainArray
{
	int size;
	uint64_t *array;
} chainArray;


typedef struct daIndex
{
	bucketHashTable *bucket;
	chainArray *chain;
} daIndex;


int bucketHashFunction(uint64_t);

chainArray *chainArrayCreateInit(int);
bucketHashTable *bucketTableCreateInit(void);

daIndex **DAIndexArrayCreate(bucket_array *);
daIndex *DAIndexCreate(bucket *);
void DAIndexArrayDestroy(daIndex **,int);
void DAIndexDestroy(daIndex *);

int DAIndexInsert(daIndex *, uint64_t, int);

#endif

#ifdef __cplusplus
}
#endif