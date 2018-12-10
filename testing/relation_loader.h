#ifndef _RELATION_LOADER_H
#define _RELATION_LOADER_H
//#include "../header/structs.h"
#include <stdint.h>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

typedef struct Relations{
	uint64_t **relation;
	uint64_t size;
	uint64_t numColumns;
}Relations;

typedef struct relationArray
{
	std::vector<Relations*> relations;
}relationArray;

void deleteRelations(relationArray** );
relationArray* init_relations();
Relations* load_relations(const char*);

#endif