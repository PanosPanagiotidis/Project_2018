#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "helper_functions.h"
#include "randarr.h"
#include "daindex.h"
#include "results.h"

int main(void){

	srand((unsigned) time(NULL));

	int rowsA = 400;
	int rowsB = 800;

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


	print_results(r);

	destroy_results(&r);



	DAIndexArrayDestroy(I,Table_A->bck_array->size);
	Destroy_Table_Data(&Table_A);
	Destroy_Table_Data(&Table_B);


	free(IdA);
	free(IdB);
	free(Payload_A);
	free(Payload_B);

	return 0;


}
