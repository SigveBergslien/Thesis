#pragma once
#include "Types.h"

class PlayerData
{
public:
	//Setting default values incase no data is entered
	PlayerData():health(1),points(0)
	{
		//Empty
	};
	~PlayerData()
	{
		//Empty
	};

	int health;
	int points;

};