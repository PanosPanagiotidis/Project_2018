#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../header/relation_loader.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void init_relations(const char* filepath)
{	
	int docs = 0;
	FILE *fp;
	size_t len = 0;
	char* buf;
	char** doc_table;


	fp = fopen("filepaths.txt","r");

	if (fp == NULL){
		fprintf(stderr,"Could not open requested file");
		exit(1);
	}

	// while(!feof(fp)){
	// 	if(fgetc(fp) == '\n'){
	// 		docs++;
	// 	}
	// }

	while(getline(&buf,&len,fp) > 0 )
		docs++;


	rewind(fp);

	doc_table = malloc(sizeof(char*) * docs);

	for(int i = 0 ; i < docs ; i++){
		doc_table[i] = NULL;

		getline(&doc_table[i],&len,fp);
		doc_table[i][strlen(doc_table[i]) -1 ] = '\0';
		printf("doc is %s\n",doc_table[i]);
	}

	load_relations(doc_table[0]);

}

void load_relations(const char* filename)
{
	int fd = open(filename,O_RDONLY);
	if(fd == -1)
		fprintf(stderr,"Could not open requested file \n");
	

	//Get file size

	struct stat sb;
	if(fstat(fd,&sb) == -1)
		fprintf(stderr,"fstat error\n");


	long length = sb.st_size;
	char* addr = (char*)(mmap(0,length,PROT_READ,MAP_PRIVATE | MAP_POPULATE,fd,0));

	if(addr == MAP_FAILED)
		fprintf(stderr,"Cannot mmap %s of length %ld\n",filename,length);

	if(length < 16)
		fprintf(stderr,"Relation file %s does not contain a valid header\n",filename);


	u_int64_t size = *((u_int64_t*)addr);
	addr+=sizeof(size);
	u_int64_t num_columns =*((u_int64_t*)addr);
	

	printf("rows is %ld and columns is %ld\n",size,num_columns);


	for(int i = 0 ; i < num_columns ; i++){
		for(int j = 0; j < size; j++)
		{
			addr+=sizeof(u_int64_t);
			printf("%ld ", *((u_int64_t*)addr 	));
			printf("|");
		}
		printf("\n");
	}
}