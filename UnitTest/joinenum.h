#ifndef _JOINENUM_H_
#define _JOINENUM_H_

#include "relation_loader.h"
#include "parser.h"
#include <unordered_set>


typedef struct treeHashTable
{
	std::unordered_set<int> **sets;		// array containing sets
	std::vector<int> **trees;			// array containing vectors (trees)
	int size;
} treeHashTable;

treeHashTable *initializeTable(relationArray *);


std::vector<int> *joinEnumeration(relationArray *, Query *);

int setHashFunction(treeHashTable *, std::unordered_set<int> *);
int hashFunct(int,int);
int hashTableUpdate(treeHashTable *, std::unordered_set<int> *, std::vector<int> *);
std::vector<int> *hashTableGet(treeHashTable *, std::unordered_set<int> *);

std::vector< std::unordered_set<int> *> *getSubSets(std::unordered_set<int>, int);
std::unordered_set<int> *createSet(relationArray *);
std::vector<int> *createJoinTree(std::vector<int> *, int);

uint64_t getCost(std::vector<int> *, relationArray *, Query *);
columnStats **calculateJoinStats(relationArray *,int,int,int,int, columnStats *, columnStats *);
int isConnected(std::unordered_set<int>,int,int **,int);
int **genConnectArray(Query *qr);

void debugPrintSet(std::unordered_set<int>);
void debugPrintVector(std::vector<int>);
void debugPrintConArr(int **connectedArray, int size);
void debugPrintTHT(treeHashTable *tht);

void destroyTreeHashTable(treeHashTable *);
void destroyConnectArray(int **, uint64_t);

void optimizeQuery(relationArray*, Query *);


#endif