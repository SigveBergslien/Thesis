#pragma once
#include "Types.h"

class HeightMapData
{
public:
	//Setting default values incase no data is entered
	HeightMapData():maxDiff(2048),passes(9),random(64),border(5),baseRadius(20),routeRadius(5),routePoints(10),sector(16)
	{
		//Empty
	};
	~HeightMapData()
	{
		//Empty
	};

	int maxDiff; 
	int passes; 
	int random; 
	int border;
	int baseRadius;
	int routeRadius;
	int routePoints;
	int sector;

};

