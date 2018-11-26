//#include "unity.h"
//#include "../header/randarr.h"
#include <stdlib.h>
#include <stdio.h>
#include "../header/relation_loader.h"
#include "../header/structs.h"


// void test_empty(void)
// {
// 	int32_t *arr = malloc(sizeof(int32_t)*0);
// 	TEST_ASSERT_EQUAL_INT32_ARRAY(arr, create_column(0,0), 0); 
// }

// void test_one(void)
// {
// 	int32_t arr[1];
// 	arr[0] = 1;
// 	TEST_ASSERT_EQUAL_INT32_ARRAY(arr, create_column(1,1), 1); 
// }

// void test_doc_paths(void)
// {
// 	int totaldocs = 14;
// }

int main(void)
{	
	// const int init_size = 15;
	// Table_Info **Relations = malloc(sizeof(Table_Info*) * init_size);

	// for(int i = 0 ; i  < init_size ; i ++ ){
	// 	Relations[i] = malloc(sizeof(Table_Info));
	// }
	//Table_Info *T = malloc(sizeof(Table_Info));
	Table_Info **Relations;
	init_relations(Relations);

	// for(int i = 0 ; i < 3 ; i++){
	// 	for(int j = 0 ; j < 1561 ; j++){
	// 		printf("%ld 	|",T->relation[i][j]);
	// 	}
	// 	printf("\n ---------------new column-------------------\n");
	// }
}