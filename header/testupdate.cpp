void tempResultsJoinUpdate1(uint64_t ** joinResults,int relationID1, int relationID2, int foundFlag1, int foundFlag2, uint64_t resultSize, tempResults **tpr)
{
	std::vector<tempResultArray>::iterator it;

	if( foundFlag1 == 0 && foundFlag2 == 0)												// Neither relation exists on tempresults
	{																					// Create a new tempresult array with both
		tempResultArray temp;

		temp.rowID.push_back(joinResults[0]);

		temp.rowID.push_back(joinResults[1]);

		temp.relationID.push_back(relationID1);
		temp.relationID.push_back(relationID2);

		temp.size = resultSize;

		(*tpr)->res.push_back(temp);

	}
	else if (foundFlag1 == 1 && foundFlag2 ==0)
	{
		for( it = (*tpr)->res.begin(); it != (*tpr)->res.end(); it++)
		{
				std::vector<int>::iterator it1;
				std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
				for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++)
					if( (*it1) == relationID1 )//VRIKA TON PINAKA STA TEMP RESULTS
						break;

				(*it2)= joinResults[0];
				(*it).relationID.push_back(relationID2);
				(*it).rowID.push_back(joinResults[1]);
				(*it).size = resultSize;
		}
	}
	else if(foundFlag2==1 && foundFlag1 ==0)
	{
		for( it = (*tpr)->res.begin(); it != (*tpr)->res.end(); it++)
		{
				std::vector<int>::iterator it1;
				std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
				for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++)
					if( (*it1) == relationID2 )//VRIKA TON PINAKA STA TEMP RESULTS
						break;

				(*it2) = joinResults[1];
				(*it).relationID.push_back(relationID1);
				(*it).rowID.push_back(joinResults[0]);
				(*it).size = resultSize;
		}		
	}
	else
	{
		for( it = (*tpr)->res.begin(); it != (*tpr)->res.end(); it++)
		{
				std::vector<int>::iterator it1;
				std::vector<int>::iterator it3;
				std::vector<uint64_t *>::iterator it2 = (*it).rowID.begin();
				std::vector<uint64_t *>::iterator it4 = (*it).rowID.begin();
				for( it1 = (*it).relationID.begin(); it1 != (*it).relationID.end(); it1++,it2++)
					if( (*it1) == relationID1 )//VRIKA TON PINAKA STA TEMP RESULTS
						break;

				for( it3 = (*it).relationID.begin(); it3 != (*it).relationID.end(); it3++,it4++)
					if( (*it3) == relationID2 )//VRIKA TON PINAKA STA TEMP RESULTS
						break;

				(*it2)= joinResults[0];
				(*it4)= joinResults[1];
				(*it).size = resultSize;
		}				
	}
}