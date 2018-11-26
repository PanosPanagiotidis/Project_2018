#include <stdio.h>
#include "../header/parser.h"


void QueryInput(){
	size_t len = 0;
	ssize_t nread = 0;
	char* line;

	while(1){
		getline(&line,&len,stdin);
		line[strlen(line)-1] = '\0';
		if(!strcmp(line,"DONE")) break;

		if(!strcmp(line,"F")){
			printf("enter new query\n");
			continue;
		} 

		ParseQuery(line);

		if(line != NULL){
			free(line);
			line = NULL;
		}


	}

}

void ParseQuery(char* query){

}