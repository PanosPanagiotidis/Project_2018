#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#define N 8


int main(void){
	int32_t a[3][3] = {{1,1,1},{1,5,2},{10,1,6}};
	int32_t b[3][3] = {{1,1,1},{1,5,2},{10,1,6}};

	int32_t bits = (1 << N);

	char mask = (1 << bits) - 1;

	int32_t reorderedArray[3][3];


}