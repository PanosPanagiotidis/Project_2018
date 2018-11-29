
#ifndef _PARSER_H
#define _PARSER_H
//#include "../header/structs.h"
#include <vector>
#include <string>

using namespace std;

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

typedef struct queryListNode{															// Linked List Node
	Query *qr;
	struct queryListNode *next;
}queryListNode;

typedef struct queryList{																// Singly Linked List containing queries
	std::vector<string> queries;												// Pointer to last query for fast insertions
} queryList;

int insertNodeInQueryList(queryList *,Query *);
queryListNode *queryListNodeCreate(Query *);
void queryListDestroy(queryList *);

queryList *QueryInput();
Query *ParseQuery(char* );
int* get_relation_array(char* ,int* );
Query* create_query(char* );
checksum_views* get_views(char* );

#endif
