#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../header/relation_loader.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

Relations* init_relations()
{
	int docs = 0;
	FILE *fp;
	size_t len = 0;
	char* buf;
	char** doc_table;


	char* line = NULL;
	ssize_t nread;

	int doc = 0;
	int init_t_size = 10;

	doc_table = malloc(sizeof(char*)*init_t_size);

	while(getline(&line,&len,stdin)){
		line[strlen(line)-1] = '\0';
		if(!strcmp(line,"DONE"))
			break;

		if(doc >= init_t_size){
			doc_table = realloc(doc_table,init_t_size+10);
			init_t_size +=10;
		}

		doc_table[doc] = strdup(line);
		doc++;
		if(line != NULL){
			free(line);
			line = NULL;
		}

	}


	// for(int i = 0 ; i < doc ; i++){
	// 	printf("doc is in %s\n",doc_table[i]);
	// }	
	Relations* R;

	R = load_relations(doc_table,doc);

	return R;
}

Relations* load_relations(char** doc_table,int doc)
{	

	Relations *R;
	R = malloc(sizeof(Relations) * doc);


	int fd = -1;
	struct stat sb;
	long length = 0;
	char* addr = NULL;
	char* tmpaddr = NULL;
	uint64_t size = -1;

	for(int cdoc = 0 ; cdoc < doc ; cdoc++){
		fd = open(doc_table[cdoc],O_RDONLY);
		if(fd == -1)
			fprintf(stderr,"Could not open requested file\n");

		if(fstat(fd,&sb) == -1)
			fprintf((stderr), "fstat error\n");

		length = sb.st_size;
		addr = (char*)(mmap(0,length,PROT_READ,MAP_PRIVATE | MAP_POPULATE,fd,0));

		tmpaddr = addr;

		if(tmpaddr == MAP_FAILED)
			fprintf(stderr,"Cannot mmap %s of length %ld\n",doc_table[cdoc],length);

		if(length < 16)
			fprintf(stderr,"Relation file %s does not contain a valid header\n",doc_table[cdoc]);

		//printf("%p\n",tmpaddr);

		size = *((u_int64_t*)tmpaddr);
		printf("size is %ld\n",size);
		tmpaddr+=sizeof(size);
		uint64_t num_columns =*((u_int64_t*)tmpaddr);
		tmpaddr = tmpaddr + sizeof(u_int64_t);

		R[cdoc].relation = malloc(sizeof(u_int64_t *) * num_columns);
		R[cdoc].rows = num_columns;
		R[cdoc].cols = size;

		for(int i = 0 ; i < num_columns ; i++){
				R[cdoc].relation[i] = malloc(sizeof(u_int64_t)*size);
		}

		printf("rows is %ld and columns is %ld\n",size,num_columns);


		for(int i = 0 ; i < num_columns ; i++)
		{
			for(int j = 0 ; j < size ; j++)
			{
			//	printf("%ld |", *((u_int64_t*)tmpaddr) );

				// DO THE THING
				R[cdoc].relation[i][j] = *((u_int64_t*)tmpaddr);
				//printf("%ld 	|",R[cdoc]->relation[i][j]);
				tmpaddr+=sizeof(u_int64_t);
			}
			//printf("\n -----new row---- \n");
			//tmpaddr = tmpaddr - 3*size*sizeof(u_int64_t);
			//tmpaddr = tmpaddr + sizeof(u_int64_t);
		}

		munmap(addr,length);

	}

	return R;

}