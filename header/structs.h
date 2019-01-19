#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdint.h>
#include <vector>

typedef struct toumble{
	uint64_t key;//rowId
	uint64_t payload;//data
	uint64_t *rids;
}toumble;


typedef struct relation{
	toumble *tuples;
	uint64_t num_tuples;
}relation;

void* RadixHashJoin(relation *reIR,relation *reIS); //temporary void

typedef struct bucket{
	toumble **tuplesArray;
	int size;
}bucket;

typedef struct Bucket_Array{
	bucket **bck;
	int size;
}bucket_array;

typedef struct Table_Info
{
	uint64_t **relation;
	toumble **tuples_table;
	int rows;
	int cols;
	uint64_t* pSum;
	uint64_t* pSumDsp;
	uint64_t* histogram;
	uint64_t* R_Id;
	uint64_t* R_Payload;
	uint64_t histSize;
	bucket_array* bck_array;	//this needs to be bucket** fix later

	int relNum;
	uint64_t**rids;
}Table_Info;

typedef struct Tables{
	Table_Info** Table_Array;
}Tables;




#endif
