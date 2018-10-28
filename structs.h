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
}bucket;

typedef struct Bucket_Array{
  bucket **bucketArray;
}bucket_array;

#endif
