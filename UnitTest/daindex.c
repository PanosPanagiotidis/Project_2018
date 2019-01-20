#include <stdio.h>
#include <stdlib.h>
#include "daindex.h"
#include <gtest/gtest.h>
#include "gmock/gmock.h"

chainArray *chainArrayCreateInit(int size)												// Creates and initializes a chainArray
{
	chainArray *chain = (chainArray*)malloc(sizeof(chainArray));
	chain->array  = (uint64_t*)calloc(size,sizeof(uint64_t));
	chain->size   = size;
	return chain;
}
TEST(chainArrayCreateInit,size)
{
	EXPECT_EQ(10, chainArrayCreateInit(10)->size);
}

// test case for the array(not working)
// TEST(chainArrayCreateInit,array) 
// {
// 	uint64_t x[3] = {0,0,0};
// 	EXPECT_EQ(ElementsAreArray({0,0,0}),chainArrayCreateInit(3)->array);
// }

TEST(chainArrayCreateInit,array)
{
	uint64_t x[3] = {0,0,0};
	EXPECT_EQ(x, chainArrayCreateInit(3)->array);
}

bucketHashTable *bucketTableCreateInit(void)											// Creates and initializes 'bucket' hash table
{
	bucketHashTable *bucket = (bucketHashTable*)malloc(sizeof(bucketHashTable));
	bucket->size = HASHFUNC_RANGE;														// Bucket array's size is equal to the range of the hash function
	//bucket->table =  malloc((bucket->size)*sizeof(bucketHashTableData));
	bucket->table = (uint64_t*)malloc((bucket->size)*sizeof(uint64_t));
	bucket->occupiedCount = 0;

	//for(int i=0; i< bucket->size; i++ )	bucket->table[i].position = 0;				// Marking all 'bucket' entries as empty
	for(int i=0; i<bucket->size; i++)	bucket->table[i] = 0;

	return bucket;
}
TEST(bucketTableCreateInit, size)
{
	EXPECT_EQ(11, bucketTableCreateInit()->size);
}
TEST(bucketTableCreateInit, occupiedCount)
{
	EXPECT_EQ(0, bucketTableCreateInit()->occupiedCount);
}
TEST(bucketTableCreateInit,table)
{
	uint64_t *x = (uint64_t*)malloc(11*sizeof(uint64_t));
	EXPECT_EQ(x, bucketTableCreateInit()->table);
}

daIndex **DAIndexArrayCreate(bucket_array *bckArray)									// Creates and returns an daIndex for each bucket in bckArray
{
	int bckCount = bckArray->size;

	daIndex **indArray = (daIndex**)malloc(bckCount*sizeof(daIndex *));

	for(int i=0; i<bckCount; i++)														// Create each daIndex
	{
		indArray[i] = DAIndexCreate(bckArray->bck[i]);

		if(indArray[i] == NULL)															// If a daIndex could not be created
		{
			for(int j=0; j<i; j++)	DAIndexDestroy(indArray[j]);						// Free all previously created indices
			free(indArray);
			return NULL;
		}
	}

	return indArray;
}


daIndex *DAIndexCreate(bucket *buck)													// Creates and returns the index of a bucket
{
	int bucketSize = buck->size;

	daIndex* bcktIndex = (daIndex*)malloc(sizeof(daIndex));

	bcktIndex->chain = chainArrayCreateInit(bucketSize+1);								// Craeting & initializing chain array
																						// Chain Array's size is equal to the bucket's plus 1 ( 1-based indexing )
	bcktIndex->bucket = bucketTableCreateInit();										// Initializing 'Bucket' Hash Table


	for(int i=0; i<bucketSize; i++ )													// Tries to insert each data the bucket contains into the index
	{
		uint64_t data = buck->tuplesArray[i]->payload;
		if( DAIndexInsert(bcktIndex,data,i) )
		{
			fprintf(stderr, "Data Insertion Error.\n");
			DAIndexDestroy(bcktIndex);
			return NULL;
		}
	}

	return bcktIndex;
}

void DAIndexArrayDestroy(daIndex **indArray, int size)
{
	if(indArray == NULL)	return;
	for(int i=0; i< size; i++)
	{
		if(indArray[i] == NULL)	break;
		DAIndexDestroy(indArray[i]);
	}
	free(indArray);
}


void DAIndexDestroy(daIndex *indx)														// Frees all memory allocated for a daIndex
{
	if( indx == NULL)	return;
	if( indx->chain != NULL)
	{
		free(indx->chain->array);
		free(indx->chain);
	}
	free(indx->bucket->table);
	free(indx->bucket);
	free(indx);
}



int DAIndexInsert(daIndex *bcktIndex, uint64_t data, int i)								// Inserts the 'i'th data to the index
{
	int hashValue = bucketHashFunction(data);

	if( bcktIndex->bucket->table[hashValue] == 0 )										// If bucket array entry is empty, the data is inserted
		bcktIndex->bucket->table[hashValue]	= i+1;										// with array index equal to its hash value
	else
	{
		int tmp = bcktIndex->bucket->table[hashValue];									// Else, the start of the chainArray chain is updated to the new index
		bcktIndex->bucket->table[hashValue] = i+1;
		bcktIndex->chain->array[i+1] = tmp;
	}
	return 0;
}

int bucketHashFunction(uint64_t data)													// Hash function used to map R data to array "Bucket"
{
	return data % HASHFUNC_RANGE;														// A simple modulo for now
}

TEST(bucketHashFunction, ok)
{
	ASSERT_EQ(10, bucketHashFunction(10));
	ASSERT_EQ(6, bucketHashFunction(50));
	ASSERT_EQ(1, bucketHashFunction(45));
}
