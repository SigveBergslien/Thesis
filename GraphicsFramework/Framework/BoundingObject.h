#pragma once
#include "Vector3.h"

class BoundingObject{
public:
	BoundingObject(Vector3 pos){position = pos;}
	virtual ~BoundingObject(){}
	Vector3 position;
	void updatePosition(Vector3 pos){position = pos;}
};