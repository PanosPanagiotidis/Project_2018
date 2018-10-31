#include "helper_functions.h"
#define N 4
// bucket* create_bucket(int size){
// 	bucket *arr = malloc(sizeof(bucket));
// 	arr->tuplesArray = malloc(sizeof(tuple) * size);//allocate space for array of buckets
// 	return arr;
// }

// Table_Info* init_table_info(int* a, int* b, int size)	// Initializes the variables and structs of table info, a=keys, b=payloads
// {
// 	Table_Info* ti = malloc(sizeof(Table_Info));

// 	tuple** t = malloc(sizeof(tuple*)*size);			// Creating tuple array (rowID,value)
// 	for(int i = 0; i < size; i++)
// 	{
// 		ti->table[i]->key = a[i];
// 		ti->table[i]->payload = b[i];
// 	}
// 	// ti->table = ti;

// 	ti->histSize = 1 << N;
// 	ti->histogram= (int*)calloc(ti->histSize,sizeof(int));
// 	ti->pSum = (int*)malloc(sizeof(int*)*ti->histSize);
// 	ti->buckets = malloc(sizeof(bucket_array));
// 	ti->buckets = malloc(sizeof(bucket*) * (ti->histSize));
// 	for(int i =0; i < ti->histSize; i++)
// 		ti->buckets[i] = malloc(sizeof(bucket));
// }