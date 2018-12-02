
#ifndef _PARSER_H
#define _PARSER_H
//#include "../header/structs.h"
#include <vector>
#include <string>

using namespace std;

enum p_type{JOIN,EQ_FILTER,GT_FILTER,LT_FILTER};

typedef struct predicates{
	enum p_type type;
	int relation1;
	int relation2;
	int column1;
	int column2;
	int filter;

}predicates;


typedef struct checksum_views{
	int rel_views;
	int rel_cols;
}checksum_views;

typedef struct Query{
	std::vector<predicates*> p;
	std::vector<int> relations;
	std::vector<checksum_views*> checksums;

}Query;

typedef struct queryBatch{
	Query **queries;
	int queryCount;
}queryBatch;

queryBatch * QueryInput();
Query *ParseQuery(string );
int* get_relation_array(char* ,int* );
Query* create_query(char* );
checksum_views* get_views(char* );

#endif
