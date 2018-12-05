#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../header/relation_loader.h"


relationArray* init_relations()
{
	std::string line;
	relationArray *R = new relationArray;
	   		std::cout << "Enter file path" << std::endl;
   	while (getline(std::cin, line)) {
      if (line == "Done" || line == "DONE" || line == "done") break;
      R->relations.push_back(load_relations(line.c_str()));
   	}

   	return R;
}

Relations* load_relations(const char* fileName)
{

	Relations *R = new Relations;

	int fd = open(fileName, O_RDONLY);
	if (fd==-1) {
	std::cerr << "cannot open " << fileName << std::endl;
	throw;
	}

	// Obtain file size
	struct stat sb;
	if (fstat(fd,&sb)==-1)
	std::cerr << "fstat\n";

	auto length=sb.st_size;
	char* tempadr;
	char* addr=static_cast<char*>(mmap(nullptr,length,PROT_READ,MAP_PRIVATE,fd,0u));
	tempadr = addr;
	if (addr==MAP_FAILED) {
	std::cerr << "cannot mmap " << fileName << " of length " << length << std::endl;
	throw;
	}

	if (length<16) {
	std::cerr << "relation file " << fileName << " does not contain a valid header" << std::endl;
	throw;
	}

	R->size=*reinterpret_cast<uint64_t*>(addr);
	addr+=sizeof(R->size);
	R->numColumns=*reinterpret_cast<int*>(addr);
	addr+=sizeof(size_t);

	R->relation = new uint64_t*[R->numColumns];

	for(uint64_t i = 0 ; i < R->numColumns ; i ++){
		R->relation[i] = new uint64_t[R->size];
	}

	for (unsigned i=0;i<R->numColumns;++i) {
		for(unsigned j = 0 ; j < R->size ; j++){
			R->relation[i][j] = *((uint64_t*)addr);
			addr+=sizeof(uint64_t);
		}
	}


	munmap(tempadr,length);

	return R;
}