#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "../header/parser.h"
#include <fstream>
#include <streambuf>

using namespace std;


queryBatch * QueryInput(){
	string line;
	vector<string> queryInput;
	int queryCount = 0;


	cout << "Enter your query" << endl;

	
	// ifstream t("wrong_input.txt");
	// ifstream t("empty_input.txt");																// svhnw to while loop giati kanw insert apo txt file
	ifstream t("correct_input.txt");
	string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	cout << str << endl;
	line = str; 
	queryInput.push_back(line);
	queryCount++;
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
	char c;

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
		ac +=2;
		pr->column1 = atoi(array+ac);
		ac++;
		if(array[ac] == '='){
			c = array[ac+2];
			if(c == '.'){
				pr->filter = JOIN;
				ac++;
				pr->relation2 = atoi(array+ac);
				ac+=2;
				pr->column2 = atoi(array+ac);
			}else if(c != '.'){
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

		cv->rel_views = atoi(array+ac);
		ac+=2;
		cv->rel_cols =	atoi(array+ac);
		ac+=2;

		query->checksums.push_back(cv);

	}

	delete(start);
	return query;

}