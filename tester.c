#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "helper_functions.h"
#include "randarr.h"
#include "daindex.h"
#include "results.h"

int main(void){

	int rowsA = 4000;
	int rowsB = 8000;

	int32_t* IdA;
	int32_t* IdB;

	int32_t* Payload_A;
	int32_t* Payload_B;


	IdA = create_column(rowsA,1);
	IdB = create_column(rowsB,1);

	Payload_A = create_column(rowsA,0);
	Payload_B = create_column(rowsB,0);

	Table_Info *Table_A = init_table_info(IdA,Payload_A,rowsA);
	Table_Info *Table_B = init_table_info(IdB,Payload_B,rowsB);

	daIndex **I = DAIndexArrayCreate(Table_A->bck_array);


	result *r = getResults(Table_A,Table_B,I);
	//int loop = r->size;
	//int i = 0;
	// while(r->size > 0 ){
	// 	printf("Result on row %d and row %d",r->results_array[i].key,r->results_array[i].payload);
	// 	loop--;
	// 	i++;
	// }

	for(int i = 0; i < Table_A->bck_array->size; i++)
	{
		printf("\n--BUCKET %d!!!\n\n",i);
		for(int j = 0; j < Table_A->bck_array->bck[i]->size; j++)
		{
			int lel = Table_A->bck_array->bck[i]->tuplesArray[j]->payload;
			printf("Position: %d  Size: Payload %d, Chain: %d, Modulo: %d\n", j+1, lel , I[i]->chain->array[j+1], lel % 11 );
		}

		printf("boketarray:\n\n");

		for(int j=0; j< I[i]->bucket->size; j++)
		{
			printf("Position: %d   BucketArray: %d\n",j,I[i]->bucket->table[j]);
		}

	}




	DAIndexArrayDestroy(I,Table_A->bck_array->size);
	Destroy_Table_Data(&Table_A);


	free(IdA);
	free(IdB);
	free(Payload_A);
	free(Payload_B);

	return 0;


}