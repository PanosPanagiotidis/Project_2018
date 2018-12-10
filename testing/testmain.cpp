#include "parser.h"
#include <iostream>
#include "relation_loader.h"

using namespace std;

int main()
{
	// queryBatch * B = QueryInput();

	// string str = "0 2 4|0.1=1.2&1.0=2.1&0.1>3000|0.0 1.1";
	// Query * Q = ParseQuery(str);
	// cout << Q->checksums.at(0) << endl;
	// cout << Q->relations.at(0) << endl;
	// cout << Q->p.at(0) << endl;

	// relationArray* R = init_relations();
	// cout << &R << endl;
	// cout << R->relations.at(0) << endl;

	Relations* A = load_relations("files.txt");
}