#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct dataInits{
	int rows;
	int cols;
	int** array;
}dataInits;

dataInits* create_random_array();