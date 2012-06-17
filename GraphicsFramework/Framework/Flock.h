
#include "Vector3.h"
#include "RigidBody.h"
#include "BoundingSphere.h"

#pragma once

using namespace std;

class Flock
{
public :
	Flock ();
	~ Flock (){};
	void initializeFlock(RigidBody** iflockBod, int isize);
	Vector3 allignment(RigidBody* bodyArr[], RigidBody* b);
	Vector3 seperation(RigidBody* bodyArr[], RigidBody* b);
	Vector3 cohesion(RigidBody* bodyArr[], RigidBody* b);
	void flockCheck(RigidBody* bodyArr[], RigidBody* b, int msec);
	float Flock::LengthSq(const Vector3& q);
	void updateFlock(int msec);	
	void setFlockPos(Vector3 fp) {flockPos = fp; kingPos = false;}
	Vector3 GetAvgPos() {return averagePos;}
	void resetFlock(){kingPos = true;}

private :
	RigidBody** flockBod;
	int size;
	Vector3 averagePos;
	Vector3 flockPos;
	bool kingPos;
};