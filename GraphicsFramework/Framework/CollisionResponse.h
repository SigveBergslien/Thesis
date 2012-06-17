#pragma once

#include "Vector3.h"
#include "CollisionData.h"
#include "RigidBody.h"

class CollisionResponse{
public:
	CollisionResponse(){}
	void Collide(CollisionData* col, RigidBody& a, RigidBody& b);
	void Collide(CollisionData* col, RigidBody& a);
	void CollideI(CollisionData* col, RigidBody& a, RigidBody& b);
	void CollideI(CollisionData* col, RigidBody& a);
	void CollideIC(CollisionData* col, RigidBody& a, RigidBody& b);
};