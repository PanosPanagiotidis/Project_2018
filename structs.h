typedef struct tuple{
  int32_t key;
  int32_t payload;
}tuple;


typedef struct relation{
  tuple *tuples;
  uint32_t num_tuples;
}relation;

void* RadixHashJoin(relation *reIR,relation *reIS); //temporary void

typedef struct bucket{
  int** data;
}bucket;

typedef struct Bucket_Array{
  bucket *data;
}bucket_array;
