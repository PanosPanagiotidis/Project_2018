#include <stdio.h>
#include <stdlib.h>
#include "daindex.h"
#include "structs.h"

chainArray *chainArrayCreateInit(int size)												// Creates and initializes a chainArray
{
	chainArray *chain = malloc(sizeof(chainArray));
	chain->array  = malloc(size*sizeof(int));
	chain->size   = size;
	return chain;
}

bucketHashTable *bucketTableCreateInit(void)											// Creates and initializes 'bucket' hash table
{
	bucketHashTable *bucket = malloc(sizeof(bucketHashTable));
	bucket->size = HASHFUNC_RANGE;														// Bucket array's size is equal to the range of the hash function
	bucket->table =  malloc((bucket->size)*sizeof(bucketHashTableData));
	bucket->occupiedCount = 0;

	for(int i=0; i< bucket->size; i++ )	bucket->table[i].position = 0;					// Marking all 'bucket' entries as empty

	return bucket;
}

daIndex **DAIndexArrayCreate(bucket_array *bckArray)									// Creates and returns an daIndex for each bucket in bckArray
{
	int bckCount = bckArray->size;

	daIndex **indArray = malloc(bckCount*sizeof(daIndex *));

	for(int i=0; i<bckCount; i++)														// Create each daIndex
	{
		indArray[i] = DAIndexCreate(bckArray->bucketArray[i]);

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

	daIndex* bcktIndex = malloc(sizeof(daIndex));

	bcktIndex->chain = chainArrayCreateInit(bucketSize+1);								// Craeting & initializing chain array
																						// Chain Array's size is equal to the bucket's plus 1 ( 1-based indexing )
	bcktIndex->bucket = bucketTableCreateInit();										// Initializing 'Bucket' Hash Table


	for(int i=0; i<bucketSize; i++ )													// Tries to insert each data the bucket contains into the index
	{
		int32_t data = buck->tuplesArray[i]->payload;
		if( DAIndexInsert(bcktIndex,data,i) )
		{
			fprintf(stderr, "Data Insertion Error.\n");
			DAIndexDestroy(bcktIndex);
			return NULL;
		}
	}

	return bcktIndex;
}


void DAIndexDestroy(daIndex *indx)														// Frees all memory allocated for a daIndex
{
	free(indx->chain->array);
	free(indx->chain);
	free(indx->bucket->table);
	free(indx->bucket);
	free(indx);
}


int DAIndexInsert(daIndex *bcktIndex, int32_t data, int i )								// Inserts the 'i'th data to the index
{
	int hashValue = bucketHashFunction(data);
	int dataIndex = hashValue;

	if( bcktIndex->bucket->table[dataIndex].position == 0 )								// If bucket array entry is empty, the data is inserted
	{																					// with array index equal to its hash value
		bcktIndex->bucket->table[dataIndex].position = i+1;
		bcktIndex->bucket->table[dataIndex].unhashedData = data;
	}
	else
	{
		if( bcktIndex->bucket->table[dataIndex].unhashedData == data )					// If bucket array entry contains data with the same data
		{																				// then the start of the chainArray chain is updated to the new index
			int tmp = bcktIndex->bucket->table[dataIndex].position;
			bcktIndex->bucket->table[dataIndex].position = i+1;

			bcktIndex->chain->array[i+1] = tmp;
		}
		else																			// However, if not, that means there's a collision which is resolved using linear probing
		{																				// try inserting until an empty cell (or the matching entry) is found
			dataIndex++;
			if(dataIndex>=bcktIndex->bucket->size)	dataIndex = 0;						// If the end of the table is reached, loop over to the start
			if(dataIndex == hashValue )				return 1;							// If hashvalue is reached again, hash table is full and functions fails

			if( bcktIndex->bucket->table[dataIndex].position == 0 )						// Empty bucket entry
			{
				bcktIndex->bucket->table[dataIndex].position = i+1;
				bcktIndex->bucket->table[dataIndex].unhashedData = data;
				return 0;
			}
			else if( bcktIndex->bucket->table[dataIndex].unhashedData == data )			// Entry contains same data value
			{
				int tmp = bcktIndex->bucket->table[dataIndex].position;
				bcktIndex->bucket->table[dataIndex].position = i+1;

				bcktIndex->chain->array[i+1] = tmp;
				return 0;
			}
		}
	}
	return 0;
}


int bucketHashFunction(int32_t data)													// Hash function used to map R data to array "Bucket"
{
	return data % HASHFUNC_RANGE;														// A simple modulo for now
}