#ifndef _PARSER_H
#define _PARSER_H
#include "../header/structs.h"

enum p_type{JOIN,FILTER};

typedef struct predicates{
	enum p_type type;
	int relation1;
	int relation2;
	int column1;
	int column2;
	int filter;

}predicates;


typedef struct checksum_views{
	int* rel_views;
	int* rel_cols;
}checksum_views;

typedef struct Query{
	predicates* p;
	int total_p;
	checksum_views* checksums;
	
}Query;


void QueryInput();
void ParseQuery(char* );
int* get_relation_array(char* ,int* );
Query* create_query(char* );
checksum_views* get_views(char* );

#endif