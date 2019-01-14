#include "../header/joinenum.h"
#include <iostream>
#include <math.h>

void joinEnumeration(relationArray *rArray, Query *qr)
{
	treeHashTable *tht = initializeTable(rArray);

	for( int i = 0; i < (int) rArray->relations.size()-1; i++)
	{

	}

}

/*
Algorithm:

:Get All S-Sized Subsets of Set A:

	while( size of Set A >= S ) do:
		Remove arbitrarily chosen element e from set A;
		Get All (S-1)-Sized Subsets of Set A; 	(Recursion)
		Add e to each of these Sets;
		Add the resulting sets to Result;

	return Result;
*/

std::vector< std::unordered_set<int> *> *getSubSets(std::unordered_set<int> *set, int subSetLength)
{
	std::vector< std::unordered_set<int> *> *returnVector = new std::vector< std::unordered_set<int> *>;

	while( ( (int) set->size() ) >= subSetLength )
	{
		std::unordered_set<int>::iterator it;

		it = set->begin();
		int ele = (*it);
		set->erase(it);

		// recursively get subsetlength-1 length sets
		// insert ele to these sets
		// add them to return vector
	}

	return returnVector;
}


treeHashTable *initializeTable(relationArray *rArray)
{
	treeHashTable *tht = new treeHashTable;

	int powerSetSize = pow(2, rArray->relations.size());

	tht->sets  = new std::unordered_set<int> *[powerSetSize] ();
	tht->trees = new std::vector<int> *[powerSetSize] ();

	for( int i = 0; i < (int) rArray->relations.size(); i++)
	{
		std::unordered_set<int> *unset = new std::unordered_set<int>;
		std::vector<int> *vec = new std::vector<int>;

		unset->insert(i);
		vec->push_back(i);

		hashTableUpdate(tht, unset, vec);
	}

	return tht;

}


int hashTableUpdate(treeHashTable *tht, std::unordered_set<int> *set, std::vector<int> *tree)
{
	int hf = setHashFunction(tht, set);

	if( (*tht->sets[hf]) == (*set) )
	{
		tht->trees[hf] = tree;
		return 0;
	}
	else if( tht->sets[hf] == NULL )
	{
		tht->sets[hf] = set;
		tht->trees[hf] = tree;
		return 0;
	}

	int i = hf + 1;

	while( i != hf )
	{
		if( i >= tht->size )	i = 0;

		if( (*tht->sets[i]) == (*set) )
		{
			tht->trees[i] = tree;
			return 0;
		}
		else if( tht->sets[i] == NULL )
		{
			tht->sets[i] = set;
			tht->trees[i] = tree;
			return 0;
		}
		i++;
	}

	return 1;
}

std::vector<int> *hashTableGet(treeHashTable *tht, std::unordered_set<int> *set)
{
	int hf = setHashFunction(tht, set);

	if( (*tht->sets[hf]) == (*set) )	return tht->trees[hf];
	else if( tht->sets[hf] == NULL )	return NULL;

	int i = hf+1;

	while( i != hf )
	{
		if( i >= tht->size )	i = 0;

		if( (*tht->sets[i]) == (*set) )	return tht->trees[i];
		else if( tht->sets[i] == NULL )	return NULL;

		i++;
	}
	return NULL;
}

int setHashFunction(treeHashTable *tht, std::unordered_set<int> *set)
{
	int hashFunctionValue = 0;

	std::unordered_set<int>::iterator it;

	for( it = set->begin(); it != set->end(); it++ )
	{
		int temp = 0;
		temp = hashFunct((*it) , tht->size);
		hashFunctionValue = hashFunctionValue ^ temp;
	}

	return hashFunctionValue;
}

int hashFunct(int value, int size)
{
	return value % size;
}