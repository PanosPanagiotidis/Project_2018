#include <stdlib.h>
#include "daindex.h"
#include "structs.h"

daIndex **createDAIndexArray(bucket_array *bckArray)							// Creates and returns an index for each bucket in bckArray
{
	int bckCount = bckArray->size;
	daIndex **indArray = malloc(bckCount*sizeof(daIndex *));

	for(int i=0; i<bckCount; i++)												// Create each index
		indArray[i] = createDAIndex(bckArray->bucketArray[i]);

	return indArray;
}

daIndex *createDAIndex(bucket *buck)											// Creates and returns and index of a bucket
{
	int bckSize = buck->size;

	daIndex* bckIndex = malloc(sizeof(daIndex));
	bckIndex->chain = malloc(sizeof(chainArray));

	bckIndex->chain->array = malloc(bckSize*sizeof(int));						// Allocating memory for chain array
	bckIndex->chain->size = bckSize;											// Chain Array has size equal to the bucket

	for(int i=0; i<bckSize; i++ )
	{
		
	}


	return 0;
}