#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//#include "structs.h"
#include "randarr.h"
#include "helper_functions.c"

#define N 4
#define HASH_SIZE 509


int main(void){
	//int32_t a[3][2] = {{16,1},{30,5},{10,1}};
	//int32_t b[3][2] = {{1,1},{1,5},{10,1}};

	int rows = 50;

	int32_t* IdA;
	int32_t* IdB;

	int32_t* Payload_A;
	int32_t* Payload_B;



	IdA = create_column(rows,1);
	IdB = create_column(rows,1);

	Payload_A = create_column(rows,0);
	Payload_B = create_column(rows,0);

	int **bucket_hash_table;
	int32_t bits = (1 << N);
	int i,j;
	


	int32_t  mask = (1 << N) - 1;


	//int32_t reorderedArray[3][3];

	int hist_size = 1 << N;//also TOTAL NUMBER OF BUCKETS

	int *histogram_A = (int*)calloc(hist_size,sizeof(int));
	int *histogram_B = (int*)calloc(hist_size,sizeof(int));

	int *pSum_A = (int*)malloc(sizeof(int)*hist_size);
	int *pSum_B = (int*)malloc(sizeof(int)*hist_size);

	int *pSumDsp_A = (int*)malloc(sizeof(int)*hist_size);
	int *pSumDsp_B = (int*)malloc(sizeof(int)*hist_size);

	int32_t LSB_A;//keep least significant bytes w. size < 32 bytes;
	int32_t LSB_B;//keep least significant bytes w. size < 32 bytes;


	for(i = 0 ; i < rows ; i++){
		LSB_A = Payload_A[i] & mask;
		histogram_A[LSB_A]++;

		LSB_B = Payload_B[i] & mask;
		histogram_B[LSB_B]++;
	}


	pSum_A[0]=0; //prefix sums start from 0.pSum is the sum of all items with the same last n significant bits
	pSum_B[0]=0; //prefix sums start from 0.pSum is the sum of all items with the same last n significant bits
	//pSumDsp[0]=0;

	for(i = 1 ; i < hist_size; i++){
		pSum_A[i] = pSum_A[i-1] + histogram_A[i-1];
		pSum_B[i] = pSum_B[i-1] + histogram_B[i-1];

		pSumDsp_A[i] = pSumDsp_A[i-1] + histogram_A[i-1];
		pSumDsp_B[i] = pSumDsp_B[i-1] + histogram_B[i-1];
	}


	int32_t* R_A_Payload = malloc(sizeof(int32_t)*rows); 
	int32_t* R_B_Payload = malloc(sizeof(int32_t)*rows); 
	
	int32_t* R_A_Id = malloc(sizeof(int32_t)*rows); 
	int32_t* R_B_Id = malloc(sizeof(int32_t)*rows); 

	for(i = 0; i < rows ; i++){
		LSB_A = Payload_A[i] & mask;
		R_A_Payload[pSumDsp_A[LSB_A]] = IdA[i];//key
		R_A_Id[pSumDsp_A[LSB_A]] = Payload_A[i];//payload

		LSB_B = Payload_B[i] & mask;
		R_B_Payload[pSumDsp_B[LSB_B]] = IdB[i];//key
		R_B_Id[pSumDsp_B[LSB_B]] = Payload_B[i];//payload

		pSumDsp_A[LSB_A]++;
		pSumDsp_B[LSB_B]++;
	}

	//create buckets for tables
	bucket_array *A = (bucket_array*)malloc(sizeof(bucket_array));
	bucket_array *B = (bucket_array*)malloc(sizeof(bucket_array));
	A->bucketArray = (bucket**)malloc(sizeof(bucket*) * hist_size);
	B->bucketArray = (bucket**)malloc(sizeof(bucket*) * hist_size);
	for(i = 0 ; i < hist_size ; i++){
		A->bucketArray[i] = malloc(sizeof(bucket));
		B->bucketArray[i] = malloc(sizeof(bucket));
	}


	tuple *t = malloc(sizeof(tuple));
	//maybe also a tuples array is better?
	int prgA = 0;
	int prgB = 0;
	bucket *bck;

	for (i = 0 ; i < hist_size ; i++){//check periptwsh opou to bucket den iparxei
		bck = A->bucketArray[i];
		if(histogram_Α[i] != 0){
			printf("i is %d and hist[i] is %d \n",i,histogram_Α[i]);
		bck->tuplesArray = (tuple**)malloc(sizeof(tuple*) * histogram_Α[i]);
		for(j = 0 ; j < histogram_Α[i] ; j++){
			bck->tuplesArray[j] = malloc(sizeof(tuple));
			bck->tuplesArray[j]->key = IdA[prgA];
			bck->tuplesArray[j]->payload = Payload_A[prgA];
			prgA++;
		}

	}

	}

	for (i = 0 ; i < hist_size ; i++){//check periptwsh opou to bucket den iparxei
		bck = Β->bucketArray[i];
		if(histogram_Β[i] != 0){
			printf("i is %d and hist[i] is %d \n",i,histogram_Β[i]);
		bck->tuplesArray = (tuple**)malloc(sizeof(tuple*) * histogram_Β[i]);
		for(j = 0 ; j < histogram_Β[i] ; j++){
			bck->tuplesArray[j] = malloc(sizeof(tuple));
			bck->tuplesArray[j]->key = IdB[prgB];
			bck->tuplesArray[j]->payload = Payload_B[prgB];
			prgB++;
		}

	}

	}


	// *bucket_hash_table = malloc(sizeof(int*) * HASH_SIZE); //Assigning space for buckets.An array,with size HASH_SIZE,of arrays.
	//
	// for(i = 0 ; i < hist_size ; i++){
	// 	for(j = 0 ; j < pSum[i])
	// }

/*Debug print */
// for( i=0; i< rows; i++){
// 	printf("|%d|%d=%x|%d|\n",i,R[i][0],R[i][0] & mask,R[i][1]);
// }

}
