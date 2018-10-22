#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "structs.h"

#define N 4



int main(void){
	int32_t a[3][2] = {{16,1},{30,5},{10,1}};
	int32_t b[3][2] = {{1,1},{1,5},{10,1}};

	int32_t bits = (1 << N);
	int i;
	int rows = 3;


	int32_t  mask = (1 << N) - 1;


	int32_t reorderedArray[3][3];

	int hist_size = 1 << N;

	int *histogram = calloc(hist_size,sizeof(int));
	int *pSum = malloc(sizeof(int)*hist_size);

	int *pSumDsp = malloc(sizeof(int)*hist_size);

	int32_t LSB;//keep least significant bytes w. size < 32 bytes;


	for(i = 0 ; i < rows ; i++){
		LSB = a[i][0] & mask;
		printf(" Oreganol: %x Last Bits: %x\n",a[i][0],LSB);
		histogram[LSB]++;

	}

	pSum[0]=0;
	pSumDsp[0]=0;
	for(i = 1 ; i < hist_size; i++){
		pSum[i] = pSum[i-1] + histogram[i-1];
		pSumDsp[i] = pSumDsp[i-1] + histogram[i-1];
	}


	int32_t R[3][2];

	for(i = 0; i < rows ; i++){
		LSB = a[i][0] & mask;
		R[pSumDsp[LSB]][0] = a[i][0];
		R[pSumDsp[LSB]][1] = a[i][1];

		pSumDsp[LSB]++;
	}


for( i=0; i< rows; i++){
	printf("|%d|%d=%x|%d|\n",i,R[i][0],R[i][0] & mask,R[i][1]);
}

}
