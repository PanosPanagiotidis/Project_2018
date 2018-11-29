//#include "unity.h"
//#include "../header/randarr.h"
#include <stdlib.h>
#include <stdio.h>
//#include "../header/relation_loader.h"
#include "../header/structs.h"
#include "../header/parser.h"



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
	// Relations *R;
	// R = init_relations();

	queryList *qList = QueryInput();
	// queryListNode *curr = qList->first;
	// while( curr != NULL )
	// {
	// 	printf("Checking stuff %d %d \n", curr->qr->p[0].relation1,curr->qr->p[0].relation2);
	// 	curr = curr->next;
	// }

}