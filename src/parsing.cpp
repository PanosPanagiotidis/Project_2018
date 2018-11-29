#include <string>
#include <vector>
#include <iostream>
#include "../header/parser.h"


using namespace std;


queryList* QueryInput(){
	string line;
	queryList *list = new queryList;

	cout << "Enter your query" << endl;

	while(getline(cin,line)){
		if(line == "F") break;
		list->queries.push_back(line);
	}
	vector<string>::iterator it;

	Query *qr = new Query;

	for(it = list->queries.begin(); it != list->queries.end(); it++){
		cout << *it << endl;
	}


}