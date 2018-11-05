#include "randarr.h"

/*
*	create_column creates a column based on the isId flag.
*	IDs are from 0 to r and Payloads are random.
*
*/


int* create_column(int r,int isId){	

	
	int32_t *arr;
	arr = malloc(sizeof(int32_t)*r);
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

/*
*	Creates a random array.
*	NOT IN USE
*/

dataInits* create_random_array(int r,int c){
	
	dataInits *data = malloc(sizeof(dataInits));

	time_t t;
	int **arr;
	srand((unsigned) time(NULL));


	arr = (int **)malloc(sizeof(int *) *r);
	arr[0] = (int *)malloc(sizeof(int) * c * r);

	for(int i = 0; i < r; i++)
        arr[i] = (*arr + c * i);

     for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            arr[i][j] = rand()%300;

    data->rows = r;
    data->cols = c;
    data->array = arr;
    return data;
}

