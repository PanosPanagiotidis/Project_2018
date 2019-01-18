#ifndef _RELATION_LOADER_H
#define _RELATION_LOADER_H

#include <stdint.h>
#include <vector>

#define ALLOCATION_CAP 50000000

typedef struct columnStats
{
   uint64_t minVal;
   uint64_t maxVal;
   uint64_t valueCount;
   uint64_t uniqueCount;
}columnStats;



typedef struct Relations{
	uint64_t **relation;
	uint64_t size;
	uint64_t numColumns;
 	columnStats *relationStats;
}Relations;

typedef struct relationArray
{
	std::vector<Relations*> relations;
}relationArray;

void deleteRelations(relationArray** );
relationArray* init_relations();
Relations* load_relations(const char*);

#endif