#include "randarr.h"

/*
*	create_column creates a column based on the isId flag.
*	IDs are from 0 to r and Payloads are random.
*
*/


int* create_column(int r,int isId){	

	
	int32_t *arr;
	arr = (int32_t*)malloc(sizeof(int32_t)*r);
	if(arr == NULL)
	{
		fprintf(stderr, "Could not allocate space for int array\n");
		exit(0);
	}

	for (int i = 0 ; i < r ; i++){
		if(isId){
			arr[i] = i+1;
		}else{
			arr[i] = rand()%300;
		}
	}

	return arr; 

}


