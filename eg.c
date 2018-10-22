#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "structs.h"

#define N 3



int main(void){
	int32_t a[3][2] = {{1,1},{1,5},{10,1}};
	int32_t b[3][2] = {{1,1},{1,5},{10,1}};

	int32_t bits = (1 << N);

	int rows = 3;


	int32_t  mask = (1 << N) - 1;

	// printf("%x %x\n", );

	int32_t reorderedArray[3][3];

	int hist_size = 1 << N;

	int *histogram = calloc(hist_size,sizeof(int));

	int32_t LSB;//keep least significant bytes w. size < 32 bytes;


	for(int i = 0 ; i < rows ; i++){
		LSB = a[i][0] & mask;
		histogram[LSB]++;
	}


}
