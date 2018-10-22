typedef struct tuple{
  int32_t key;
  int32_t payload;
}tuple;


typedef struct relation{
  tuple *tuples;
  uint32_t num_tuples;
}relation;

void* RadixHashJoin(relation *reIR,relation *reIS); //temporary void
