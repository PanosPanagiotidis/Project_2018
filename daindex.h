#ifndef __DA_INDEX_H__
#define __DA_INDEX_H__

#define HASH_RANGE 101

typedef struct bucketHashTable
{

} bucketHashTable;

typedef struct chainArray {

} chainArray;

typedef struct daIndex
{
    bucketHashTable bucket[HASH_RANGE];
    chainArray *chain;
} daIndex;





#endif