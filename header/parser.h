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
	
}predicates;


typedef struct Query{
	predicates* p;
	int total_p;
	
}Query;

#endif