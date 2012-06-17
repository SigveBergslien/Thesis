#pragma once

#include "Vector3.h"
#include "RigidBody.h"
#include "CollisionData.h"
#include "BoundingTriangle.h"
#include "Plane.h"
#include "Cloth.h"
#include "CollisionResponse.h"
#include <math.h>

class CollisionDetection{
public:
	CollisionDetection(){};
	bool Collision(RigidBody& a, RigidBody& b, CollisionData* colDat = NULL);
	bool Collision(BoundingSphere* a, BoundingSphere* b,CollisionData* colDat = NULL);
	bool Collision(RigidBody& a, const BoundingTriangle& b, CollisionData* colDat = NULL);
	bool Collision(BoundingSphere* a, const BoundingTriangle& b, CollisionData* colDat = NULL);
	void Collision(RigidBody& a, Cloth* b);
};