#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <stdio.h>
#include "Types.h"
#include "HeightMapData.h"
#include "PlayerData.h"
#include "GameData.h"
#include "stdlib.h"

#define MAX_DATE 12

using namespace std;

class XmlParser
{
public:
	XmlParser(){;}
	//Pass in file with data and pointer to struct which will be filled with data
	~XmlParser(){;}
	bool			Parser(string fName, void* pVoid, Type eType);		//If no file found or NULL pointer then return false
	bool			WriteToFile(string fName, void* pVoid, Type eType);
	
private:
	void			*pVoidXml;
	vector<string>	*vdatatest;
protected:
	void			FillMapData();
	void			FillPlayerData();
	void			FillData(Type eType);
	void			FillGameData();
	
	bool			WritePlayer(string fName);
	bool			WriteGameData(string fName);
};