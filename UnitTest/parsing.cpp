#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "parser.h"
#include <cctype>
#include <gtest/gtest.h>

using namespace std;


queryBatch * QueryInput(){
	string line;
	vector<string> queryInput;



	//cout << "Enter your query" << endl;

	while(getline(cin,line)){
		if(line == "F") break;
		if(line == ""){
			return NULL;
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
		test = NULL;
	}

	queryInput.erase(queryInput.begin(),queryInput.end());
	return qBatch;

}


Query* ParseQuery(string q){

	size_t pos = 0;
	string token;
	char* array = NULL;
	int ac = 0;
	int c = -1;
	char t = '0';
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
		//delete(s);
	}

	//GET PREDICATES
	q.erase(pos,q.find("|")+1);
	token = q.substr(pos,q.find("|",pos));

	istringstream isa(token);

	while(getline(isa,s,'&')){

		c = -1;
		array = NULL;
		array = strdup(s.c_str());

		ac = 0;

		predicates* pr = new predicates;
		pr->type = NA;
		pr->relation1 = -1;
		pr->relation2 = -1;
		pr->column2 = -1;
		pr->column1 = -1;
		pr->filter = -1;

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
				pr->type = JOIN;
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

		free(array);

		if(pr->type != JOIN) query->p.insert(query->p.begin(),pr);
		else query->p.push_back(pr);

	}

	array = NULL;
	q.erase(pos,q.find("|")+1);
	int size = count (q.begin(),q.end(),'.');


	istringstream isc(q);

	while(getline(isc,s,' ')){
		ac = 0;
		array = strdup(s.c_str());


		checksum_views *cv = new checksum_views;
		cv->rel_cols = -1;
		cv->rel_views = -1;

		cv->rel_views = atoi(array+ac);
		//cout << "cv->rel_views"<< cv->rel_views <<endl;
		while(isalnum(array[ac]))
			ac++;
		ac++;

		cv->rel_cols = atoi(array+ac);
		//cout << "cv->relcols"<<cv->rel_cols<<endl;
		query->checksums.push_back(cv);

		free(array);

	}





	return query;

}

void deleteQuery(queryBatch** qb){
	std::vector<Query*>::iterator q;

	for(q = (*qb)->queries.begin(); q != (*qb)->queries.end(); q++){

		for(int i = 0 ; i < (*q)->p.size() ; i++){
			delete((*q)->p.at(i));
		}

		vector<predicates*>().swap((*q)->p);

		(*q)->relations.clear();
		vector<int>().swap((*q)->relations);

		for(int i = 0 ; i < (*q)->checksums.size() ; i++){
			delete((*q)->checksums.at(i));
		}

		vector<checksum_views*>().swap((*q)->checksums);

		delete((*q));


	}


	vector<Query*>().swap((*qb)->queries);

	delete((*qb));


}