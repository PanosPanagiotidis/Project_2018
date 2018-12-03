#ifdef __cplusplus
extern "C" {
#endif


#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdint.h>


typedef struct tuple{
	int32_t key;//rowId
	int32_t payload;//data
}tuple;


typedef struct relation{
	tuple *tuples;
	uint32_t num_tuples;
}relation;

void* RadixHashJoin(relation *reIR,relation *reIS); //temporary void

typedef struct bucket{
	tuple **tuplesArray;
	int size;
}bucket;

typedef struct Bucket_Array{
	bucket **bck;
	int size;
}bucket_array;

typedef struct Table_Info
{
	uint64_t **relation;
	tuple **tuples_table;
	int rows;
	int cols;
	int32_t* pSum;
	int32_t* pSumDsp;
	int32_t* histogram;
	int32_t* R_Id;
	int32_t* R_Payload;
	int32_t histSize;
	bucket_array* bck_array;	//this needs to be bucket** fix later
}Table_Info;

typedef struct Tables{
	Table_Info** Table_Array;
}Tables;




#endif

#ifdef __cplusplus
}
#endif