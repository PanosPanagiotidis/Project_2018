#include <stdio.h>
#include "randarr.h"




int main(void){
	dataInits *A = create_random_array();
	dataInits *B = create_random_array();

	int final_array[1000*1000][2];
	final_array[0][0] = 1;

	for(int i = 0 ; i < A->rows ; i++){
		for( int j = 0 ; j < B->rows ; j++){
			if ((i + j ) != 0){
				final_array[(i+j)][0] +=1;
				final_array[(i+j)][1] = (A->array[i][0]) * (B->array[j][0]);  
			}
			printf("j = %d \n",j);
		}
		printf("i = %d \n",i);
	}
}