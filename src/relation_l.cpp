#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../header/relation_loader.h"

using namespace std;

relationArray* init_relations()
{
    std::string line;
    ifstream is("files.txt");
    relationArray *R = new relationArray;
       //         std::cout << "Enter file path" << std::endl;
       // while (getline(std::cin, line)) {
    //   if (line == "Done" || line == "DONE" || line == "done") break;
    //   R->relations.push_back(load_relations(line.c_str()));
       // }
    while(getline(is,line)){
        if(is.eof())
            break;
        R->relations.push_back(load_relations(line.c_str()));
    }
       return R;
}

Relations* load_relations(const char* fileName)
{

    Relations *R = new Relations;

    int fd = open(fileName, O_RDONLY);
    if ( fd==-1 )
    {
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

    if (addr==MAP_FAILED)
    {
        std::cerr << "cannot mmap " << fileName << " of length " << length << std::endl;
        throw;
    }

    if (length<16)
    {
        std::cerr << "relation file " << fileName << " does not contain a valid header" << std::endl;
        throw;
    }

    R->size=*reinterpret_cast<uint64_t*>(addr);
    addr+=sizeof(uint64_t);
    R->numColumns=*reinterpret_cast<uint64_t*>(addr);
    addr+=sizeof(uint64_t);


    R->relation = new uint64_t*[R->numColumns];

    for(uint64_t i = 0 ; i < R->numColumns ; i ++)
    	R->relation[i] = new uint64_t[R->size];

     R->relationStats = new columnStats[R->numColumns];

    for (unsigned i=0;i<R->numColumns;++i)
    {
        uint64_t minVal = *reinterpret_cast<uint64_t*>(addr);
        uint64_t maxVal = *reinterpret_cast<uint64_t*>(addr);

    	for(unsigned j = 0 ; j < R->size ; j++)
        {
    		R->relation[i][j] = *reinterpret_cast<uint64_t*>(addr);
            addr+=sizeof(uint64_t);
            if(R->relation[i][j] > maxVal)  maxVal = R->relation[i][j];
            if(R->relation[i][j] < minVal)  minVal = R->relation[i][j];
    	}

        R->relationStats[i].minVal = minVal;
        R->relationStats[i].maxVal = maxVal;
        R->relationStats[i].valueCount = R->size;

        char *unqArray;
        uint64_t unqArraySize = maxVal - minVal + 1;
        if( unqArraySize >= ALLOCATION_CAP )
        {
            unqArraySize = ALLOCATION_CAP;
            unqArray = new char[unqArraySize] ();
            for( uint64_t j = 0; i < R->size; i++)
                 unqArray[(R->relation[i][j] - minVal) % unqArraySize] = 1;
        }
        else
        {
            unqArray = new char[unqArraySize] ();            
            for( uint64_t j = 0; j < R->size; j++)
                unqArray[R->relation[i][j] - minVal] = 1;

        }

        uint64_t uniqueCount = 0;

        for(uint64_t j = 0; j < unqArraySize; j++)
            if( unqArray[j] != 0 )  uniqueCount++;

        R->relationStats[i].uniqueCount = uniqueCount;

        delete[] unqArray;

	}

	munmap(tempadr,length);

	return R;
}

void deleteRelations(relationArray** ra){

	int i,j;

	for(i = 0; i < (*ra)->relations.size();i++){

		for(j = 0;j < ((*ra)->relations.at(i))->numColumns ; j++)
			delete[] (((*ra)->relations.at(i))->relation[j]);


		delete[] ((*ra)->relations.at(i)->relation);
        delete[] ((*ra)->relations.at(i)->relationStats);
		delete ((*ra)->relations.at(i));
	}

	std::vector<Relations*>().swap((*ra)->relations);
	delete ((*ra));

}