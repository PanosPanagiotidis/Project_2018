#include <iostream>
#include <math.h>
#include "joinenum.h"
#include "relationops.h"

std::vector<int> *joinEnumeration(relationArray *rArray, Query *qr)
{
	treeHashTable *tht = initializeTable(rArray);
	std::unordered_set<int> * set = createSet(rArray);
	int **connectArray = genConnectArray(qr);

	for( int i = 0; i < (int) rArray->relations.size()-1; i++)
	{
		std::vector< std::unordered_set<int> *>::iterator it1;
		std::vector< std::unordered_set<int> *> *subSets = getSubSets(*set,i+1);		// get subsets of size i+1

		for( it1 = subSets->begin(); it1 != subSets->end() ; it1++ )
		{
			for( std::unordered_set<int>::iterator it2 = set->begin(); it2 != set->end(); it2++ )
			{
				if( (*it1)->find((*it2)) == (*it1)->end() )								// if it2 is not member of set it1
				{
					if( !isConnected(*(*it1),(*it2),connectArray, (int) qr->relations.size() ))
						continue;

					std::vector<int> *tr = hashTableGet(tht,*it1);
					if( tr == NULL )	continue;

					std::vector<int> *currTree = createJoinTree(tr,(*it2));
					std::unordered_set<int> set2 = (**it1);
					set2.insert(*it2);

					std::vector<int> *oldTree = hashTableGet(tht,&set2);
					if( oldTree == NULL  || getCost(oldTree,rArray,qr) > getCost(currTree,rArray,qr) )
					{
						hashTableUpdate(tht, &set2, currTree);
						if( oldTree != NULL )
							delete oldTree;
					}
					else			delete currTree;
				}
			}
			delete (*it1);
		}
		delete subSets;
	}


	std::vector<int> *result = hashTableGet(tht,set);
	std::vector<int> *rvect = new std::vector<int>;

	for(std::vector<int>::iterator it = result->begin(); it != result->end(); it++)
		rvect->push_back(*it);

	destroyTreeHashTable(tht);
	destroyConnectArray(connectArray,qr->relations.size());
	delete set;

	return rvect;
}

void optimizeQuery(relationArray *rArray, Query *qr)
{
	std::vector<int> *v = joinEnumeration(rArray,qr);
	std::vector<predicates *> newp;

	std::unordered_set<int> set;

	std::vector<int>::iterator it = v->begin();
	set.insert(*it);
	it++;

	std::vector<predicates *>::iterator it2;

	while( it != v->end() )
	{
		for( it2 = qr->p.begin(); it2 != qr->p.end(); it2++)
			if( (*it2)->type == JOIN )
			{
				if( (*it2)->relation1 == (*it) && set.find((*it2)->relation2) != set.end() )
				{
					newp.push_back(*it2);
				}
				else if( (*it2)->relation2 == (*it) && set.find((*it2)->relation1) != set.end() )
				{
					newp.push_back(*it2);
				}
			}
		set.insert(*it);
		it++;
	}

	qr->p = newp;
	delete v;
}



void destroyConnectArray(int **connectArray, uint64_t size)
{
	for( uint64_t i = 0; i < size; i++)
		delete[] connectArray[i];
	delete[] connectArray;
}

void destroyTreeHashTable(treeHashTable *tht)
{
	for( int i = 0; i < tht->size; i++)
		if( tht->sets[i] != NULL )
		{
			delete tht->sets[i];
			delete tht->trees[i];
		}

	delete[] tht->sets;
	delete[] tht->trees;
	delete tht;
}


int isConnected(std::unordered_set<int> set, int relation, int **connectArray, int size)
{
	for( std::unordered_set<int>::iterator it = set.begin() ; it != set.end(); it++)
		if( connectArray[*it][relation] )	return 1;

	return 0;
}


uint64_t getCost(std::vector<int> *tree, relationArray *rArray, Query *qr)
{
	std::unordered_set<int> set;

	// Copy Stats

	columnStats **tempStats = new columnStats*[rArray->relations.size()];
	for( uint64_t i = 0; i < rArray->relations.size(); i++ )
	{
		tempStats[i] = new columnStats[rArray->relations.at(i)->numColumns];
		for( uint64_t j = 0; j < rArray->relations.at(i)->numColumns; j++ )
		{
			tempStats[i][j].minVal		= rArray->relations.at(i)->relationStats[j].minVal;
			tempStats[i][j].maxVal		= rArray->relations.at(i)->relationStats[j].maxVal;
			tempStats[i][j].uniqueCount	= rArray->relations.at(i)->relationStats[j].uniqueCount;
			tempStats[i][j].valueCount	= rArray->relations.at(i)->relationStats[j].valueCount;
		}
	}

	uint64_t cost = 0;

	for( uint64_t i = 0; i < tree->size()-1; i++ )
	{
		int rel1 = tree->at(i);
		int rel2 = tree->at(i+1);

		int transrel1 = qr->relations.at(rel1);
		int transrel2 = qr->relations.at(rel2);

		set.insert(transrel1);

		std::vector<predicates *>::iterator it;
		for( it = qr->p.begin(); it != qr->p.end(); it++)
		{
			if((*it)->type != JOIN )	continue;
			columnStats **res = NULL;
			if( (set.find(qr->relations.at((*it)->relation1)) != set.end()) && (set.find(qr->relations.at((*it)->relation2)) == set.end()) && qr->relations.at((*it)->relation2) == transrel2 )				// if exactly one of the relations is found in the set
			{
				res = calculateJoinStats(rArray,rel1,(*it)->column1,rel2,(*it)->column2,tempStats[rel1],tempStats[rel2]);

				for( uint64_t j = 0; j < rArray->relations.at(rel1)->numColumns; j++ )
				{
					tempStats[rel1][j].minVal		= res[0][j].minVal;
					tempStats[rel1][j].maxVal		= res[0][j].maxVal;
					tempStats[rel1][j].uniqueCount	= res[0][j].uniqueCount;
					tempStats[rel1][j].valueCount	= res[0][j].valueCount;
				}

				for( uint64_t j = 0; j < rArray->relations.at(rel2)->numColumns; j++ )
				{
					tempStats[rel2][j].minVal		= res[1][j].minVal;
					tempStats[rel2][j].maxVal		= res[1][j].maxVal;
					tempStats[rel2][j].uniqueCount	= res[1][j].uniqueCount;
					tempStats[rel2][j].valueCount	= res[1][j].valueCount;
				}

				cost += tempStats[rel1][0].valueCount;
			}
			else if( (set.find(qr->relations.at((*it)->relation1)) == set.end()) && (set.find(qr->relations.at((*it)->relation2)) != set.end()) && qr->relations.at((*it)->relation1) == transrel2 )
			{
				res = calculateJoinStats(rArray,rel2,(*it)->column1,rel1,(*it)->column2,tempStats[rel2],tempStats[rel1]);

				for( uint64_t j = 0; j < rArray->relations.at(rel2)->numColumns; j++ )
				{
					tempStats[rel2][j].minVal		= res[0][j].minVal;
					tempStats[rel2][j].maxVal		= res[0][j].maxVal;
					tempStats[rel2][j].uniqueCount	= res[0][j].uniqueCount;
					tempStats[rel2][j].valueCount	= res[0][j].valueCount;
				}

				for( uint64_t j = 0; j < rArray->relations.at(rel1)->numColumns; j++ )
				{
					tempStats[rel1][j].minVal		= res[1][j].minVal;
					tempStats[rel1][j].maxVal		= res[1][j].maxVal;
					tempStats[rel1][j].uniqueCount	= res[1][j].uniqueCount;
					tempStats[rel1][j].valueCount	= res[1][j].valueCount;
				}

				cost += tempStats[rel1][0].valueCount;
			}

			if( res != NULL )
			{
				delete[] res[0];
				delete[] res[1];
				delete[] res;
			}

		}
	}

	for( uint64_t i = 0; i < rArray->relations.size(); i++ )
		delete[] tempStats[i];

	delete[] tempStats;
	return cost;
}


std::vector<int> *createJoinTree(std::vector<int> *S, int Rj)
{
	std::vector<int> *v = new std::vector<int>;

	for( std::vector<int>::iterator it= S->begin(); it != S->end(); it++)
		v->push_back(*it);
	v->push_back(Rj);

	return v;
}



std::unordered_set<int> *createSet(relationArray *rArray)
{
	std::unordered_set<int> * returnSet = new std::unordered_set<int>;
	for(int i = 0; i < (int) rArray->relations.size(); i++ )
		returnSet->insert(i);

	return returnSet;
}



treeHashTable *initializeTable(relationArray *rArray)
{
	treeHashTable *tht = new treeHashTable;

	int powerSetSize = pow(2, rArray->relations.size());

	tht->sets  = new std::unordered_set<int> *[powerSetSize] ();
	tht->trees = new std::vector<int> *[powerSetSize] ();
	tht->size = powerSetSize;

	for( int i = 0; i < (int) rArray->relations.size(); i++)
	{
		std::unordered_set<int> unset;
		std::vector<int> *vec = new std::vector<int>;

		unset.insert(i);
		vec->push_back(i);

		hashTableUpdate(tht, &unset, vec);
	}

	return tht;

}


int hashTableUpdate(treeHashTable *tht, std::unordered_set<int> *set, std::vector<int> *tree)
{
	int hf = setHashFunction(tht, set);
	std::unordered_set<int>* tmpset = new std::unordered_set<int>;

	for( std::unordered_set<int>::iterator it = set->begin(); it != set->end(); it++)
		tmpset->insert(*it);

	if( tht->sets[hf] == NULL )
	{
		tht->sets[hf] = tmpset;
		tht->trees[hf] = tree;
		return 0;
	}
	else if( (*tht->sets[hf]) == (*tmpset) )
	{
		tht->trees[hf] = tree;
		delete tmpset;
		return 0;
	}

	int i = hf + 1;
	while( i != hf )
	{
		if( i >= tht->size )	i = 0;

		if( tht->sets[i] == NULL )
		{
			tht->sets[i] = tmpset;
			tht->trees[i] = tree;
			return 0;
		}
		else if( (*tht->sets[i]) == (*tmpset) )
		{
			tht->trees[i] = tree;
			delete tmpset;
			return 0;
		}
		i++;
	}

	delete tmpset;
	return 1;
}

std::vector<int> *hashTableGet(treeHashTable *tht, std::unordered_set<int> *set)
{
	int hf = setHashFunction(tht, set);

	if( tht->sets[hf] == NULL )				return NULL;
	else if( (*tht->sets[hf]) == (*set) )	return tht->trees[hf];

	int i = hf+1;

	while( i != hf )
	{
		if( i >= tht->size )	i = 0;

		if( tht->sets[i] == NULL )				return NULL;
		else if( (*tht->sets[i]) == (*set) )	return tht->trees[i];

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


columnStats **calculateJoinStats(relationArray *relArray, int relationID1, int columnID1, int relationID2, int columnID2, columnStats *stats1, columnStats *stats2)
{
	columnStats *relationStats1, *relationStats2;
	Relations *relation1, *relation2;
	double factor;

	columnStats **results = new columnStats*[2];

	if( relationID1 != relationID2 )
	{
		relation1 = relArray->relations.at(relationID1);
		relation2 = relArray->relations.at(relationID2);

		relationStats1 = new columnStats[relation1->numColumns];
		relationStats2 = new columnStats[relation2->numColumns];


		for( uint64_t i = 0; i < relation1->numColumns; i++)
		{
			relationStats1[i].minVal = stats1[i].minVal;
			relationStats1[i].maxVal = stats1[i].maxVal;
			relationStats1[i].valueCount = stats1[i].valueCount;
			relationStats1[i].uniqueCount = stats1[i].uniqueCount;
		}

		for( uint64_t i = 0; i < relation2->numColumns; i++)
		{
			relationStats2[i].minVal = stats2[i].minVal;
			relationStats2[i].maxVal = stats2[i].maxVal;
			relationStats2[i].valueCount = stats2[i].valueCount;
			relationStats2[i].uniqueCount = stats2[i].uniqueCount;
		}


		// Applying filters as needed
		uint64_t original;
		double divisor1,divisor2;



		if( relationStats1[columnID1].maxVal > relationStats2[columnID2].maxVal )
		{
			divisor1 = relationStats1[columnID1].maxVal - relationStats1[columnID1].minVal;
			original = relationStats1[columnID1].valueCount;

			if(divisor1 == 0 )
			{
				divisor1 = 1;
				factor = ( (double) (relationStats2[columnID2].maxVal - relationStats1[columnID1].minVal))/(divisor1);
				relationStats1[columnID1].valueCount = (uint64_t) factor*relationStats1[columnID1].valueCount;
				relationStats1[columnID1].uniqueCount = 1;

			}
			else
			{
				factor = ( (double) (relationStats2[columnID2].maxVal - relationStats1[columnID1].minVal))/(divisor1);
				relationStats1[columnID1].maxVal = relationStats2[columnID2].maxVal;
				relationStats1[columnID1].valueCount = (uint64_t) factor*relationStats1[columnID1].valueCount;
				relationStats1[columnID1].uniqueCount = (uint64_t) factor*relationStats1[columnID1].uniqueCount;

			}


			for( uint64_t i = 0 ; i < relation1->numColumns ; i++ )
			{
				divisor1 = original;
				divisor2 = relationStats1[i].uniqueCount;

				if(divisor1 == 0 )	divisor1 = 1;
				if(divisor2 == 0 )	divisor2 = 1;

				factor = (1 - pow((1 - ((double) relationStats1[columnID1].valueCount)/(divisor1)),(((double)relationStats1[i].valueCount)/(divisor2))));
				relationStats1[i].uniqueCount = (uint64_t) factor*relationStats1[i].uniqueCount;
				relationStats1[i].valueCount  = relationStats1[columnID1].valueCount;
			}

		}
		else if( relationStats1[columnID1].maxVal < relationStats2[columnID2].maxVal )
		{
			factor = ((double)(relationStats1[columnID1].maxVal - relationStats2[columnID2].minVal))/((double)(relationStats2[columnID2].maxVal - relationStats2[columnID2].minVal));
			original = relationStats2[columnID2].valueCount;

			relationStats2[columnID2].maxVal = relationStats1[columnID1].maxVal;
			relationStats2[columnID2].valueCount = (uint64_t) factor*relationStats2[columnID2].valueCount;
			relationStats2[columnID2].uniqueCount = (uint64_t) factor*relationStats2[columnID2].uniqueCount;

			for( uint64_t i = 0 ; i < relation2->numColumns ; i++ )
			{
				factor = (1 - pow((1 - ((double)relationStats2[columnID2].valueCount)/((double) original)),(((double)relationStats2[i].valueCount)/((double)relationStats2[i].uniqueCount))));
				relationStats2[i].uniqueCount = (uint64_t) factor*relationStats2[i].uniqueCount;
				relationStats2[i].valueCount  = relationStats2[columnID1].valueCount;
			}

		}


		if( relationStats1[columnID1].minVal > relationStats2[columnID2].minVal )
		{
			factor = ( (double) (relationStats2[columnID2].maxVal - relationStats1[columnID1].minVal))/((double) (relationStats2[columnID2].maxVal - relationStats2[columnID2].minVal));
			original = relationStats2[columnID2].valueCount;

			relationStats2[columnID2].minVal = relationStats1[columnID1].minVal;
			relationStats2[columnID2].valueCount = (uint64_t) factor*relationStats2[columnID2].valueCount;
			relationStats2[columnID2].uniqueCount = (uint64_t) factor*relationStats2[columnID2].uniqueCount;

			for( uint64_t i = 0 ; i < relation2->numColumns ; i++ )
			{
				factor = (1 - pow((1 - ((double) relationStats2[columnID2].valueCount)/((double) original)),(((double) relationStats2[i].valueCount)/((double) relationStats2[i].uniqueCount))));
				relationStats2[i].uniqueCount = (uint64_t) factor*relationStats2[i].uniqueCount;
				relationStats2[i].valueCount  = relationStats2[columnID1].valueCount;
			}

		}
		else if( relationStats1[columnID1].minVal < relationStats2[columnID2].minVal )
		{
			factor = ((double) (relationStats1[columnID1].maxVal - relationStats2[columnID2].minVal))/((double) (relationStats1[columnID1].maxVal - relationStats1[columnID1].minVal));
			original = relationStats1[columnID1].valueCount;

			relationStats1[columnID1].minVal = relationStats2[columnID2].minVal;
			relationStats1[columnID1].valueCount = (uint64_t) factor*relationStats1[columnID1].valueCount;
			relationStats1[columnID1].uniqueCount = (uint64_t) factor*relationStats1[columnID1].uniqueCount;

			for( uint64_t i = 0 ; i < relation1->numColumns ; i++ )
			{
				factor = (1 - pow((1 - ((double) relationStats1[columnID1].valueCount)/((double) original)),(((double) relationStats1[i].valueCount)/((double) relationStats1[i].uniqueCount))));
				relationStats1[i].uniqueCount = (uint64_t) factor*relationStats1[i].uniqueCount;
				relationStats1[i].valueCount  = relationStats1[columnID1].valueCount;
			}

		}


		uint64_t original1 = relationStats1[columnID1].uniqueCount;
		uint64_t original2 = relationStats2[columnID2].uniqueCount;

		factor = ((double) (relationStats2[columnID2].valueCount))/((double) (relationStats1[columnID1].maxVal - relationStats1[columnID1].minVal + 1));

		relationStats1[columnID1].valueCount = (uint64_t) ((relationStats1[columnID1].valueCount)*factor);
		relationStats2[columnID2].valueCount = relationStats1[columnID1].valueCount;

		factor = ((double) (relationStats2[columnID2].uniqueCount))/((double) (relationStats1[columnID1].maxVal - relationStats1[columnID1].minVal + 1));

		relationStats1[columnID1].uniqueCount = (uint64_t) ((relationStats1[columnID1].uniqueCount)*factor);
		relationStats2[columnID2].uniqueCount = relationStats1[columnID1].uniqueCount;


		for(uint64_t i = 0; i < relation1->numColumns; i++)
			if( i != (uint64_t) columnID1 )
			{
				factor = ( 1 - pow(( 1 - ((double) relationStats1[columnID1].uniqueCount)/((double) original1) ),(((double) relationStats1[i].valueCount)/((double) relationStats1[i].uniqueCount))) );
				relationStats1[i].valueCount = relationStats1[columnID1].valueCount;
				relationStats1[i].uniqueCount = (uint64_t) (relationStats1[i].uniqueCount*factor);
			}

		for(uint64_t i = 0; i < relation2->numColumns; i++ )
			if( i != (uint64_t) columnID2)
			{

				factor = ( 1 - pow(( 1 - ((double) relationStats2[columnID2].uniqueCount)/((double) original2) ),(((double) relationStats2[i].valueCount)/((double) relationStats2[i].uniqueCount))) );
				relationStats2[i].valueCount = relationStats2[columnID2].valueCount;
				relationStats2[i].uniqueCount = (uint64_t) (relationStats2[i].uniqueCount*factor);
			}


		results[0] = relationStats1;
		results[1] = relationStats2;

	}
	else
	{
		relation1 = relArray->relations.at(relationID1);
		relationStats1 = new columnStats[relation1->numColumns];

		for( uint64_t i = 0; i < relation1->numColumns; i++)
		{
			relationStats1[i].minVal = stats1[i].minVal;
			relationStats1[i].maxVal = stats1[i].maxVal;
			relationStats1[i].valueCount = stats1[i].valueCount;
			relationStats1[i].uniqueCount = stats1[i].uniqueCount;
		}


		if( columnID1 != columnID2 )
		{
			relationStats1[columnID1].maxVal = max(relationStats1[columnID1].maxVal,relationStats1[columnID2].maxVal);
			relationStats1[columnID2].maxVal = relationStats1[columnID1].maxVal;

			relationStats1[columnID1].minVal = min(relationStats1[columnID1].minVal,relationStats1[columnID2].minVal);
			relationStats1[columnID2].minVal = relationStats1[columnID1].minVal;

			uint64_t original = relationStats1[columnID1].valueCount;

			relationStats1[columnID1].valueCount = (uint64_t) (((double) (original))/((double) (relationStats1[columnID1].maxVal - relationStats1[columnID1].minVal + 1)));
			relationStats1[columnID2].valueCount = relationStats1[columnID1].valueCount;

			factor = 1 - pow(( 1 - (((double) relationStats1[columnID1].valueCount)/((double) original))),(((double) original)/(relationStats1[columnID1].uniqueCount)));

			relationStats1[columnID1].uniqueCount = (uint64_t) factor*relationStats1[columnID1].uniqueCount;
			relationStats1[columnID2].uniqueCount = relationStats1[columnID1].uniqueCount;

		}
		else
		{
			relationStats1[columnID1].valueCount = (uint64_t) (((double) ((relationStats1[columnID1].valueCount)*(relationStats1[columnID1].valueCount)))/((double) (relationStats1[columnID1].maxVal - relationStats1[columnID1].minVal + 1)) );

			for( uint64_t i = 0; i < relation1->numColumns; i++)
				relationStats1[i].valueCount = relationStats1[columnID1].valueCount;

		}

		results[0] = relationStats1;
		results[1] = NULL;
	}

	return results;
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

std::vector< std::unordered_set<int> *> *getSubSets(std::unordered_set<int> set, int subSetLength)
{
	std::vector< std::unordered_set<int> *> *returnVector = new std::vector< std::unordered_set<int> *>;
	std::vector< std::unordered_set<int> *> *tempVector = NULL;

	while( subSetLength > 0 && ( (int) set.size() ) >= subSetLength  )
	{
		std::unordered_set<int>::iterator it;

		it = set.begin();
		int ele = (*it);
		set.erase(it);

		// recursively get subsetlength-1 length sets
 		tempVector = getSubSets(set,subSetLength-1);


		// insert ele to each of them and add them to the return vector
		std::vector< std::unordered_set<int> *>::iterator it1;
		for( it1 = tempVector->begin(); it1 != tempVector->end(); it1++ )
		{
			(*it1)->insert(ele);
			returnVector->push_back(*it1);
		}

		// delete
		while(!tempVector->empty())
			tempVector->pop_back();

		delete tempVector;
	}

	if( subSetLength == 0 )
	{
		std::unordered_set<int> *set2 = new std::unordered_set<int>;
		returnVector->push_back(set2);
	}

	return returnVector;
}




int **genConnectArray(Query *qr)
{
	int **connectArray = new int*[qr->relations.size()];

	for( uint64_t i = 0; i < qr->relations.size(); i++ )
	{
		connectArray[i] = new int[qr->relations.size()]();
		connectArray[i][i] = 1;
	}

	std::vector<predicates*>::iterator it;

	for( it = qr->p.begin(); it != qr->p.end(); it++ )
	{
		if( (*it)->type != JOIN )	continue;
		connectArray[(*it)->relation1][(*it)->relation2] = 1;
		connectArray[(*it)->relation2][(*it)->relation1] = 1;

	}
	return connectArray;
}

void debugPrintSet(std::unordered_set<int> set)
{
	cout << "Size is " << set.size() << "   ";
	for(std::unordered_set<int>::iterator it = set.begin(); it != set.end(); it++)
		cout << (*it) << " ";

	cout << endl;
}

void debugPrintVector(std::vector<int> vec)
{
	for(std::vector<int>::iterator it = vec.begin(); it != vec.end(); it++ )
		cout << *it << " ";

	cout << endl;
}

void debugPrintConArr(int **connectedArray, int size)
{
	for( int i = 0; i < size; i++)
	{
		cout << i << " ";
		for( int j = 0; j < size; j++)
		{
			if( connectedArray[i][j] )	cout << " X ";
			else						cout << "   ";
		}
		cout << endl;
	}
}


void debugPrintTHT(treeHashTable *tht)
{
	for(int i = 0; i < tht->size; i++)
	{
		if( tht->sets[i] != NULL)
		{
			debugPrintSet(*tht->sets[i]);
			debugPrintVector(*tht->trees[i]);
		}
		cout << endl;
	}
}
