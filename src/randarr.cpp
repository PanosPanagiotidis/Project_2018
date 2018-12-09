#include "randarr.h"
#include <vector>

/*
*	create_column creates a column based on the isId flag.
*	IDs are from 0 to r and Payloads are random.
*
*/

using namespace std;

std::vector<uint64_t> create_column(int r,int isId){

	std::vector<uint64_t> array;


	for (int i = 0 ; i < r ; i++){
		if(isId){
			array.push_back(i+1);
		}else{
			array.push_back(rand()%100);
		}
	}

	return array;

}

//*/*/
