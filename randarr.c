#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	time_t t;
	int **arr;
	srand((unsigned) time(&t));

	int r = rand()%100 + 1;
	int c = rand()%100 + 2;
	arr = (int **)malloc(sizeof(int *) *r);
	arr[0] = (int *)malloc(sizeof(int) * c * r);

	for(int i = 0; i < r; i++) 
        arr[i] = (*arr + c * i);

     for (int i = 0; i < r; i++) 
        for (int j = 0; j < c; j++) 
            arr[i][j] = rand()%300;
}