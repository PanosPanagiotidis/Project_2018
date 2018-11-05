#include "unity.h"
#include "randarr.h"
#include <stdlib.h>
#include <stdio.h>

void test_empty(void)
{
	int32_t *arr = malloc(sizeof(int32_t)*0);
	TEST_ASSERT_EQUAL_INT32_ARRAY(arr, create_column(0,0), 0); 
}

void test_one(void)
{
	int32_t arr[1];
	arr[0] = 1;
	TEST_ASSERT_EQUAL_INT32_ARRAY(arr, create_column(1,1), 1); 
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_empty);
	RUN_TEST(test_one);
	return UNITY_END();
}