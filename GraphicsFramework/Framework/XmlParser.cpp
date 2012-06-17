#include "XmlParser.h"
#include <time.h>
bool XmlParser::Parser(string fName, void* pVoid, Type eType)
{
	//Parser in the data from the file provided into the data structure for the user to use
	ifstream fileIn(fName);
	
	if (fileIn.is_open() && pVoid)
	{
		pVoidXml = pVoid; //Make tge pointer exsited in this class
		string line;
		string startTag;
		string endTag;
		string data;
		bool endTagFlag = false;
		vector<string> vdata;

		while(fileIn.good())
		{
			getline(fileIn,line);
			//test the line for the elements and data and split them up
			const char *p = line.c_str();
			while (*p != '\0') 
			{
				if(*p == '<')
				{
					p++;
					if(*p == '?')
						break;
					while(*p != '>')
					{
						if(*p == '/' || endTagFlag)
						{
							endTagFlag = true;
							endTag.append(1,*p++);
						}
						else
						{
							startTag.append(1,*p++);
						}
					}

					p++;
					if(endTagFlag)
					{
						endTag.clear();
					}
					else
					{
						startTag.clear();
					}

					endTagFlag = false;

					while(*p != '<' && *p != '\0')
					{
						data.append(1,*p++);
					}	
									
					if( *p !='\0' && *p == '<')
					{
						vdata.push_back(data);
						data.clear();
					}
				}
				
				if(*p == ' ')
					p++;
			}
		  
		}
		fileIn.close();
		//Call function to switch though the type and call the correct fill function
		vdatatest = &vdata;
		FillData(eType);
	}
	else
	{
		return false;
	}
	return true;
}

//Call this method to write data back to an XML file
bool XmlParser::WriteToFile(string fName, void* pVoid, Type eType)
{
	if(!pVoid)
		return false;
	
	pVoidXml = pVoid;	//Make the pointer exsited in this class
	
	switch(eType)
	{
		case PLAYER:
			if(WritePlayer(fName)){
				return true;
			}
			else{
				return false;
			}
			break;
		case MAP:
			break;
		case GAME:
			if(WriteGameData(fName)){
				return true;
			}
			else{
				return false;
			}
			break;
		default:
			break;
	}

	return false;
}

//switch though the types and call the correct fill function
void XmlParser::FillData(Type eType)
{
	switch(eType)
	{
	case MAP:
			FillMapData();
			break;
	case PLAYER:
			FillPlayerData();
			break;
	case GAME:
			FillGameData();
			break;
	default:
		break;
	}
}

void XmlParser::FillMapData()
{
	//Cast the void pointer into hightmap data;
	HeightMapData *pMapData = (HeightMapData*)pVoidXml;
	
	pMapData->maxDiff = atoi((*vdatatest)[0].c_str());
	pMapData->passes = atoi((*vdatatest)[1].c_str());
	pMapData->random = atoi((*vdatatest)[2].c_str());
	pMapData->border = atoi((*vdatatest)[3].c_str());
	pMapData->baseRadius = atoi((*vdatatest)[4].c_str());
	pMapData->routeRadius = atoi((*vdatatest)[5].c_str());
	pMapData->routePoints = atoi((*vdatatest)[6].c_str());
	pMapData->sector = atoi((*vdatatest)[7].c_str());

	//Clear up
	vdatatest->clear();

}

void XmlParser::FillPlayerData()
{
	//Cast pointer
	PlayerData *pPlayerData = (PlayerData*)pVoidXml;

	pPlayerData->points = atoi((*vdatatest)[0].c_str());
	pPlayerData->health = atoi((*vdatatest)[1].c_str());
	//Clear up
	vdatatest->clear();
}

void XmlParser::FillGameData()
{
	GameData *pGameData = (GameData*)pVoidXml;
	
	pGameData->setCrystalsCollected(atoi((*vdatatest)[0].c_str()));
	pGameData->setEnemiesKilled(atoi((*vdatatest)[1].c_str()));
	pGameData->setNumberOfPookas(atoi((*vdatatest)[2].c_str()));
	pGameData->setNumberTrees(atoi((*vdatatest)[3].c_str()));

	pGameData->setNumberPredators(atoi((*vdatatest)[4].c_str()));
	pGameData->setNumberMushrooms(atoi((*vdatatest)[5].c_str()));
	pGameData->setNumberExtraPookas(atoi((*vdatatest)[6].c_str()));
	pGameData->setNumberStartingPookas(atoi((*vdatatest)[7].c_str()));
	pGameData->setNumberFlowers(atoi((*vdatatest)[8].c_str()));
	
	//Clear up
	vdatatest->clear();
	
}

bool XmlParser::WritePlayer(string fName)
{
	PlayerData *pPlayerData = (PlayerData*)pVoidXml;

	//date-time
	char buffer[150];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	strftime(buffer,150,"%d%m%y_%H%M%S",timeinfo);
	fName.append(buffer);
	fName.append(".xml");

    ifstream ifile(fName);
	if(ifile) 
	{
		ifile.close();
		//The file exists, you can just append xml
		ofstream myfile (fName ,ios::app );
		if (myfile.is_open())
		{
			myfile << "<health>";
			myfile << pPlayerData->health;
			myfile << "</health>\n<points>";
			myfile << pPlayerData->points;
			myfile << "</points>\n";
			myfile.close();
		}
		else 
		{
			//cout << "Unable to open file: " << fName << endl;
			return false;
		}
	}
	else
	{
		//Create new file
		ofstream myfile (fName);
		if (myfile.is_open())
		{
			myfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
			myfile << "<player>\n<health>";
			myfile << pPlayerData->health;
			myfile << "</health>\n<points>";
			myfile << pPlayerData->points;
			myfile << "</points>\n</player>";
			myfile.close();
		}
		else 
		{
			return false;
			//cout << "Unable to write file: " << fName <<endl;;
		}
	}
	return true;
}

bool XmlParser::WriteGameData(string fName)
{
	GameData *pGameData = (GameData*)pVoidXml;

	//date-time
	char buffer[150];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	strftime(buffer,150,"%d%m%y_%H%M%S",timeinfo);
	fName.append(buffer);
	fName.append(".xml");

    ifstream ifile(fName);
	if(ifile) 
	{
		ifile.close();
		//The file exists, you can just append xml
		ofstream myfile (fName ,ios::app );
		if (myfile.is_open())
		{
			myfile << "<crystalsCollected>";
			myfile << pGameData->getCrystalsCollected();
			myfile << "</crystalsCollected>\n<enemiesKilled>";
			myfile << pGameData->getEnemiesKilled();
			myfile << "</enemiesKilled>\n<numberOfPookas>";
			myfile << pGameData->getNumberOfPookas();
			myfile << "</numberOfPookas>\n<numberOfTrees>";
			myfile << pGameData->getNumberTrees();
			myfile << "</numberOfTrees>\n<numberOfPredators>";
			myfile << pGameData->getNumberPredators();
			myfile << "</numberOfPredators>\n<numberOfMushrooms>";
			myfile << pGameData->getNumberMushrooms();
			myfile << "</numberOfMushrooms>\n<numberExtraPookas>";
			myfile << pGameData->getNumberExtraPookas();
			myfile << "</numberExtraPookas>\n<numberOfStartingPookas>";
			myfile << pGameData->getNumberStartingPookas();
			myfile << "</numberOfStartingPookas>\n<numberOfFlowers>";
			myfile << pGameData->getNumberFlowers();
			myfile << "</numberOfFlowers>";
			myfile.close();
		}
		else 
		{
			//cout << "Unable to open file: " << fName << endl;
			return false;
		}
	}
	else
	{
		//Create new file
		ofstream myfile (fName);
		if (myfile.is_open())
		{
			myfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
			myfile << "<gamedata>\n";
			myfile << "<crystalsCollected>";
			myfile << pGameData->getCrystalsCollected();
			myfile << "</crystalsCollected>\n<enemiesKilled>";
			myfile << pGameData->getEnemiesKilled();
			myfile << "</enemiesKilled>\n<numberOfPookas>";
			myfile << pGameData->getNumberOfPookas();
			myfile << "</numberOfPookas>\n<numberOfTrees>";
			myfile << pGameData->getNumberTrees();
			myfile << "</numberOfTrees>\n<numberOfPredators>";
			myfile << pGameData->getNumberPredators();
			myfile << "</numberOfPredators>\n<numberOfMushrooms>";
			myfile << pGameData->getNumberMushrooms();
			myfile << "</numberOfMushrooms>\n<numberExtraPookas>";
			myfile << pGameData->getNumberExtraPookas();
			myfile << "</numberExtraPookas>\n<numberOfStartingPookas>";
			myfile << pGameData->getNumberStartingPookas();
			myfile << "</numberOfStartingPookas>\n<numberOfFlowers>";
			myfile << pGameData->getNumberFlowers();
			myfile << "</numberOfFlowers>\n</gamedata>";
			myfile.close();
		}
		else 
		{
			return false;
			//cout << "Unable to write file: " << fName <<endl;;
		}
	}
	return true;
}