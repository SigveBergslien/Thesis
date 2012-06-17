#pragma once

#include "Vector3.h"

class CollisionData{
public:
	Vector3 point;
	Vector3 normal;
	float penetration;
};