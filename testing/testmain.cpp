#include "parser.h"
#include <iostream>

using namespace std;

int main()
{
	queryBatch * B = QueryInput();
	string str = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1";
	Query * Q = ParseQuery(str);
}