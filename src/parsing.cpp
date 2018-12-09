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



	cout << "Enter your query" << endl;

	while(getline(cin,line)){
		if(line == "F") break;
		if(line == ""){
			cout << "no empty strings " <<endl;
			continue;
		}
		queryInput.push_back(line);
	}
	vector<string>::iterator it;


	queryBatch *qBatch = new queryBatch;

	//qBatch->queries = new Query*[queryCount];

	int i=0;
	Query* test = NULL;


	for(it = queryInput.begin() ; it != queryInput.end() ; it++, i++){

		test = ParseQuery((*it));
		if(test == NULL){
			qBatch = NULL;
			break;
		}

		qBatch->queries.push_back(test);
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
	char t;
	int delims =0;


	for(int i = 0 ; i < q.length();i++){
		t = q[i];
		if(t == '|')
			delims++;
	}

	if(delims > 2){
		cout <<"Please enter a correct query" << endl;
		return NULL;
	}
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


		}else if(array[ac] == '>'){
			pr->type = GT_FILTER;
			ac++;
			pr->filter = atoi(array+ac);
		}else if (array[ac]=='<'){
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
		//cout << "rel->views is" << cv ->rel_views << endl;// DELET DIS
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