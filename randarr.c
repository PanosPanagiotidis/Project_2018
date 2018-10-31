#include "randarr.h"


int* create_column(int r,int isId){

	srand((unsigned) time(NULL));
	int32_t *arr;
	arr = malloc(sizeof(int32_t)*r);

	for (int i = 0 ; i < r ; i++){
		if(isId){
			arr[i] = i+1;
		}else{
			arr[i] = rand()%300;
		}
	}

	return arr; 

}

dataInits* create_random_array(int r,int c){
	dataInits *data = malloc(sizeof(dataInits));


	time_t t;
	int **arr;
	srand((unsigned) time(NULL));

	//int r = 1000;//rand()%100 + 1;
	//int c = 2;//rand()%100 + 2;
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



// int main(void)
// {
// 	dataInits *A = create_random_array();
// 	printf("A data is | rows = %d | cols = %d | \n",A->rows,A->cols);
// 	for (int i = 0 ; i < A->rows ; i++){
// 		for(int j = 0 ; j < A->cols ; j++){
// 			printf("%d ",A->array[i][j]);
// 		}
// 		printf("\n");
// 	}
// 	dataInits *B = create_random_array();
// }
