#include <stdio.h>
#include <stdlib.h>
#include "daindex.h"
#include "structs.h"

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

	bcktIndex->chain = malloc(sizeof(chainArray));										// Initializing chain array
	bcktIndex->chain->array = malloc((bucketSize+1)*sizeof(int));
	bcktIndex->chain->size = bucketSize+1;												// Chain Array's size is equal to the bucket's plus 1 ( 1-based indexing )

	bcktIndex->bucket = malloc(sizeof(bucketHashTable));								// Initializing 'Bucket' Hash Table
	bcktIndex->bucket->size = HASHFUNC_RANGE;											// Bucket array's size is equal to the range of the hash function
	bcktIndex->bucket->table = malloc(HASHFUNC_RANGE*sizeof(bucketHashTableData));
	bcktIndex->bucket->occupiedCount = 0;

	for(int i=0; i<HASHFUNC_RANGE; i++)	bcktIndex->bucket->table[i].position = 0;		// Marking all 'bucket' entries as empty


	for(int i=0; i<bucketSize; i++ )													// Tries to insert each key the bucket contains into the index
	{
		int32_t key = buck->tuplesArray[i]->key;
		if( DAIndexInsert(bcktIndex,key,i) )
		{
			fprintf(stderr, "Key Insertion Error.\n");
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


int DAIndexInsert(daIndex *bcktIndex, int32_t key, int i )								// Inserts the 'i'th key to the index
{
	int hashValue = bucketHashFunction(key);
	int keyIndex = hashValue;

	if( bcktIndex->bucket->table[keyIndex].position == 0 )								// If bucket array entry is empty, the key is inserted
	{																					// with array index equal to its hash value
		bcktIndex->bucket->table[keyIndex].position = i+1;
		bcktIndex->bucket->table[keyIndex].unhashedData = key;
	}
	else
	{
		if( bcktIndex->bucket->table[keyIndex].unhashedData == key )					// If bucket array entry contains data with the same key
		{																				// then the start of the chainArray chain is updated to the new index
			int tmp = bcktIndex->bucket->table[keyIndex].position;
			bcktIndex->bucket->table[keyIndex].position = i+1;

			bcktIndex->chain->array[i+1] = tmp;
		}
		else																			// However, if not, that means there's a collision which is resolved using linear probing
		{																				// try inserting until an empty cell (or the matching entry) is found
			keyIndex++;
			if(keyIndex>=bcktIndex->bucket->size)	keyIndex = 0;						// If the end of the table is reached, loop over to the start
			if(keyIndex == hashValue )				return 1;							// If hashvalue is reached again, hash table is full and functions fails

			if( bcktIndex->bucket->table[keyIndex].position == 0 )						// Empty bucket entry
			{
				bcktIndex->bucket->table[keyIndex].position = i+1;
				bcktIndex->bucket->table[keyIndex].unhashedData = key;
				return 0;
			}
			else if( bcktIndex->bucket->table[keyIndex].unhashedData == key )			// Entry contains same key value
			{
				int tmp = bcktIndex->bucket->table[keyIndex].position;
				bcktIndex->bucket->table[keyIndex].position = i+1;

				bcktIndex->chain->array[i+1] = tmp;
				return 0;
			}
		}
	}
	return 0;
}


int bucketHashFunction(int32_t key)														// Hash function used to map R keys to array "Bucket"
{
	return key % HASHFUNC_RANGE;														// A simple modulo for now
}