#pragma once
#include "SceneNode.h"

class Predator : public SceneNode
{
public:
	Predator(int n) :  number(n)		{};
	~Predator(void){};
	
	

	string GetType()					{return type;}
	int GetNumber()						{return number;}
protected:
	string type;
	int number;
	
};

