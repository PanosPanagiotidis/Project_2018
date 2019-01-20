#include <iostream>
#include "relation_loader.h"
#include <gtest/gtest.h>
#include <vector>
#include "daindex.h"
#include "daindex.c"
// #include "relationops.cpp"
#include "gmock/gmock.h"

int ff(int a, int b)
{
	int c = a+b;
	return c;
}


// TEST(chainArray,Init)
// {
// 	uint64_t * x[10];
// 	for(int i=0; i<10; i++)
// 	{
// 		x[i] = 0;
// 	}
// 	chainArray* c = chainArrayCreateInit(10);
// 	EXPECT_TRUE(0 == memcmp(x, c->array, 10));
// }
// TEST(ok, okk)
// {
// 	uint64_t x = {0, 1, 2, 3, 4};
// 	uint64_t* a = createRowID(5);
// 	EXPECT_EQ(x, a);
// }
TEST(ff, PositiveNos)
{
	ASSERT_EQ(5, ff(3,2));
	ASSERT_EQ(0, ff(0,0));
}
TEST(ff,NegativeNos)
{
	EXPECT_EQ(-10, ff(-8,-2));
	EXPECT_EQ(-1, ff(2,-3));
}

int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}