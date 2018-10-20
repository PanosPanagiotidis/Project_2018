#include <stdio.h>


#define N 2




int main(void){
	int a[3][3] = {{1,1,1},{1,5,2},{10,1,6}};
	int b[3][3] = {{1,1,1},{1,5,2},{10,1,6} };

	int bits = (1 << N);

	char mask = (1 << bits) - 1;

	printf("no");


}