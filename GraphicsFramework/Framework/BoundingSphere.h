#ifndef BOUNDING_SPHERE_H
#define BOUNDING_SPHERE_H
#include "BoundingObject.h"

class BoundingSphere : public BoundingObject{
public:
	BoundingSphere():BoundingObject(Vector3(0,0,0)){};
	BoundingSphere(float r, Vector3 pos):BoundingObject(pos)
	{
		position = pos;
		radius = r;
	};
	inline void initialize(float r, Vector3 pos){
		radius = r;
		BoundingObject::updatePosition(pos);
	}
	float radius;
};

#endif