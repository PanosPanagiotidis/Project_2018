#ifndef _JOINENUM_H_
#define _JOINENUM_H_

#include "../header/relation_loader.h"
#include "../header/parser.h"
#include <unordered_set>

typedef struct treeHashTable
{
	std::unordered_set<int> **sets;		// array containing sets
	std::vector<int> **trees;			// array containing vectors (trees)
	int size;
} treeHashTable;

treeHashTable *initializeTable(relationArray *);


void joinEnumeration(relationArray *, Query *);

int setHashFunction(treeHashTable *, std::unordered_set<int> *);
int hashFunct(int,int);
int hashTableUpdate(treeHashTable *, std::unordered_set<int> *, std::vector<int> *);
std::vector<int> *hashTableGet(treeHashTable *, std::unordered_set<int> *);

std::vector< std::unordered_set<int> *> *getSubSets(std::unordered_set<int> *, int);
#endif