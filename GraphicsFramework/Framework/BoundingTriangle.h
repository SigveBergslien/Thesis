#pragma once
#include "BoundingObject.h"

class BoundingTriangle{
public:
	BoundingTriangle(){}
	BoundingTriangle(Vector3 a,Vector3 b, Vector3 c){
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}
	inline void initialize(Vector3 a,Vector3 b, Vector3 c){
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}
	Vector3 vertices[3];
};