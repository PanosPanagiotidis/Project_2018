#ifndef RANDARR_H
#define RANDARR_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct dataInits{
	int rows;
	int cols;
	int** array;
}dataInits;

dataInits* create_random_array(int ,int );

int* create_column(int , int);
#endif
