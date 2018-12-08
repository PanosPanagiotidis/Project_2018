#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "../header/parser.h"
#include <cctype>

using namespace std;


queryBatch * QueryInput(){
	string line;
	vector<string> queryInput;
	int queryCount = 0;


	cout << "Enter your query" << endl;

	while(getline(cin,line)){
		if(line == "F") break;
		queryInput.push_back(line);
		queryCount++;
	}
	vector<string>::iterator it;


	queryBatch *qBatch = new queryBatch;
	qBatch->queryCount = queryCount;

	qBatch->queries = new Query*[queryCount];

	int i=0;

	for(it = queryInput.begin() ; it != queryInput.end() ; it++, i++){
		qBatch->queries[i] = ParseQuery(*it);
	}

	return qBatch;

}


Query* ParseQuery(string q){

	size_t pos = 0;
	string token;
	char* array;
	char* start;
	int ac = 0;
	int c = -1;

	Query *query = new Query;

	token = q.substr(pos,q.find("|",pos));

	istringstream iss(token);	//GET RELATIONS NEEDED
	string s;
	while(getline(iss,s,' ')){
		query->relations.push_back(stoi(s));
	}

	//GET PREDICATES
	q.erase(pos,q.find("|")+1);
	token = q.substr(pos,q.find("|",pos));

	istringstream isa(token);

	while(getline(isa,s,'&')){

		array = new char[s.length()+1];
		start = array;
		s.copy(array,s.length()+1);

		ac = 0;

		predicates* pr = new predicates;

		pr->relation1 = atoi(array+ac);

		while(isalnum(array[ac])){
			ac ++;
		}
		ac++;

		pr->column1 = atoi(array+ac);

		while(isalnum(array[ac])){
			ac++;
		}

		if(array[ac] == '='){
			if(strchr(array+ac,'.')){
				c = 1;
			}
			else{
				c = 0;
			}

			if(c == -1){
				cout << "ridiculous command given " << endl;
				exit(1);
			}
			if(c == 1){
				pr->filter = JOIN;
				ac++;
				pr->relation2 = atoi(array+ac);

				while(isalnum(array[ac])){
					ac++;
				}
				ac++;
				pr->column2 = atoi(array+ac);


			}else if(c == 0){
				pr->type = EQ_FILTER;
				ac++;
				pr->filter = atoi(array+ac);
			}


		}else if(!strcmp(array,">")){
			pr->type = GT_FILTER;
			ac++;
			pr->filter = atoi(array+ac);
		}else if (!strcmp(array,"<")){
			pr->type = LT_FILTER;
			ac++;
			pr->filter = atoi(array+ac);
		}

		delete(start);

		query->p.push_back(pr);

	}

	q.erase(pos,q.find("|")+1);
	int size = count (q.begin(),q.end(),'.');
	array = new char[q.length()+1];
	start = array;
	q.copy(array,q.length()+1);
	ac = 0;


	for(int i = 0 ; i < size ; i++){

		checksum_views *cv = new checksum_views;

		cv->rel_views = query->relations.at(atoi(array+ac));
		//cout << "rel->views is" << cv ->rel_views << endl; DELET DIS
		while(isalnum(array[ac])){
			ac++;
		}
		ac++;

		cv->rel_cols =	atoi(array+ac);

		while(isalnum(array[ac])){
			ac++;
		}

		query->checksums.push_back(cv);

	}

	delete(start);
	return query;

}