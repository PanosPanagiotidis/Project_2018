#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/parser.h"


queryList *QueryInput(){
	size_t len = 0;
	ssize_t nread = 0;
	char* line;

	printf("Enter your query\n");

	queryList *qList = malloc(sizeof(queryList));										// queryList initialization
	if( qList == NULL )	return NULL;
	qList->first = NULL;
	qList->last  = NULL;


	while(1){
		getline(&line,&len,stdin);
		line[strlen(line)-1] = '\0';
		if(!strcmp(line,"DONE")) break;

		if(!strcmp(line,"F")){
			printf("enter new query\n");
			continue;
		}

		Query *qr = ParseQuery(line);
		if(insertNodeInQueryList(qList,qr))
		{
			printf("very bad\n");
			queryListDestroy(qList);
			if(line != NULL)	free(line);
			return NULL;
		}

		if(line != NULL){
			free(line);
			line = NULL;
		}


	}

	return qList;

}

Query* ParseQuery(char* query){
	int total_p = 0;
	int i = 0;
	char* q = strdup(query);
	char* ptr = q;


	char* q_parts[3];
	char* tkn;

	tkn = strtok(q,"|");


	/*
		Get 3 parts.
		Part 1 are the relations to be acted on.
		Part 2 are the predicates.
		Part 3 are the checksum lists.
	*/

	while(tkn != NULL){
		q_parts[i] = strdup(tkn);
		tkn = strtok(NULL,"|");

		printf("part i %d is %s\n",i,q_parts[i]);
		i++;
	}

	tkn = NULL;
	int array_size;
	int *q_relations = get_relation_array(q_parts[0],&array_size);


	Query *qr = create_query(q_parts[1]);

	printf("q_parts[2] is %s\n",q_parts[2]);

	qr->checksums = get_views(q_parts[2]);


	return qr;


}


int* get_relation_array(char* tables,int* size){

	char* temp = strdup(tables);
	char* tkn = NULL;
	char delim[2] = " ";
	int count = 0;

	tkn = strtok(tables,delim);

	while(tkn != NULL){
		count++;
		tkn = strtok(NULL,delim);
	}


	int* array = malloc(sizeof(int)*count);

	int i = 0;

	tkn = strtok(temp,delim);

	while(tkn != NULL){
		array[i] = atoi(tkn);
		i++;
		tkn = strtok(NULL,delim);
	}

	*size = count;

	free(temp);

	return array;

}

//0.1=1.2&1.0=2.1&0.1>3000

Query* create_query(char* prds){

	Query* query = malloc(sizeof(query));
	char* temp = strdup(prds);
	int count = 0;
	int total_p = 0;
	char* ptr = temp;
	char* tkn = NULL;

	char delim[2] ="&";

	while((ptr = strchr(ptr,'&')) != NULL){
		total_p++;
		ptr++;
	}

	total_p++;

 	query->p = malloc(sizeof(predicates)*total_p);
 	query->total_p = total_p;
 	int i = 0;

 	tkn = strtok(temp,delim);
 	char* pred = NULL;


 	while(tkn != NULL){

 		pred = strdup(tkn);

 		query->p[i].relation1 = atoi(pred);
 		pred++;
 		pred++;
 		query->p[i].column1 = atoi(pred);
 		pred++;

 		if(strchr(pred,'=')){
 			query->p[i].type = JOIN;
 			pred++;
 			query->p[i].relation2 = atoi(pred);
 			pred++;
 			pred++;
 			query->p[i].column2 = atoi(pred);

 		}else if(strchr(pred,'>') || strchr(pred,'<')){
 			query->p[i].type = FILTER;
 			pred++;
 			query->p[i].filter = atoi(pred);
 		}

 		// printf("rel 1 is %d\n",query->p[i].relation1);
 		// printf("rel 2 is %d\n",query->p[i].relation2);

 		// printf("col 1 is %d\n",query->p[i].column1);
 		// printf("col 2 is %d\n",query->p[i].column2);

 		// printf("------------------new predicate----------------\n");
 		i++;

 		tkn = strtok(NULL,delim);
 		pred = NULL;

 	}
 	free(temp);
 	return query;


}


checksum_views* get_views(char* views){
	char* temp = strdup(views);
	char* ptr = temp;
	int total_v = 0;

	while((ptr = strchr(ptr,'.')) != NULL){
		total_v++;
		ptr++;
	}

	total_v++;
	printf("total v is %d\n",total_v);

	checksum_views *cv = malloc(sizeof(checksum_views));
	cv->rel_views = malloc(sizeof(int)*total_v);
	cv->rel_cols = malloc(sizeof(int)*total_v);

	int i = 0;

	char* tkn = NULL;

	tkn = strtok(temp," ");

	while(tkn != NULL){
		cv->rel_views[i] = atoi(tkn);
		tkn++;
		tkn++;
		cv->rel_cols[i] = atoi(tkn);

		i++;

		tkn = strtok(NULL," ");
	}

	free(temp);

	return cv;

}

int insertNodeInQueryList(queryList *qList, Query *qr)
{
	queryListNode *node = queryListNodeCreate(qr);										// Create Node to be inserted

	if(node == NULL)	return 1;														// Fails if node creation failed

	if(qList->last == NULL)																// Node insertion if list is empty
	{
		qList->first = node;
		qList->last = node;
		return 0;
	}

	qList->last->next = node;															// Else inserts node at the end of the list
	qList->last = node;

	return 0;

}

queryListNode *queryListNodeCreate(Query *qr)
{
	queryListNode *node = malloc(sizeof(queryListNode));

	if( node == NULL )	return NULL;													// Malloc failed

	node->qr = qr;
	node->next = NULL;

	return node;
}

void queryListDestroy(queryList *qList)
{
	if(qList == NULL )	return;

	queryListNode *curr = qList->first;
	queryListNode *next;
	while( curr != NULL )
	{
		next = curr->next;
		free(curr);
		curr = next;
	}

	free(qList);
}
