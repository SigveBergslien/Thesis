#include "Flock.h"
#include <iostream>

using namespace std;


Flock::Flock()
{
	flockBod = NULL;
}

void Flock::initializeFlock(RigidBody** iflockBod, int isize)
{
	flockBod = iflockBod;
	size = isize;
}

void Flock::updateFlock(int msec)
{
	for(int i = 1; i < size; i++)
		flockCheck(flockBod, flockBod[i],msec);
}

void Flock::flockCheck(RigidBody* bodyArr[], RigidBody* b, int msec)
{
	Vector3 vel;
	vel = b->GetVelocity() + (cohesion(bodyArr, b) + allignment(bodyArr, b) + seperation(bodyArr, b));
	vel.Normalise();
	b->AddVelocity(Vector3(vel.x, 0.0f, vel.z)*msec/15);
}

Vector3 Flock::allignment(RigidBody* bodyArr[], RigidBody* b)
{
	Vector3 allign;
	int vlim = 100;

	for (int i = 0; i < size; i++)
	{
		allign = allign + bodyArr[i]->GetVelocity();
	}

	allign = allign / (float)size;
	allign = allign - b->GetVelocity();

	if(Vector3::Dot(allign, allign) < vlim)
		allign.Normalise();

		allign = allign / (float)vlim;

	return allign;
}

Vector3 Flock::seperation(RigidBody* bodyArr[], RigidBody* b)
{
	Vector3 displacement;

	//for (int i = 0; i < size; i++)
	//{
		//const float distSq = (b->GetPosition() - bodyArr[i]->GetPosition()).SqrLength();
		//BoundingSphere* tmp1= (BoundingSphere*)b->getBoundingObject();
		//BoundingSphere* tmp2= (BoundingSphere*)bodyArr[i]->getBoundingObject();
		//const float sumRadius = (tmp1->radius + tmp2->radius);

		//if(distSq < sumRadius*sumRadius)
		//	displacement = displacement - (b->GetPosition() - bodyArr[i]->GetPosition());
	//}
	return Vector3(0.f,0.f,0.f);
}

Vector3 Flock::cohesion(RigidBody* bodyArr[], RigidBody* b)
{
	Vector3 coheed;
	for (int i = 0; i < size; i++)
	{
		coheed = coheed + bodyArr[i]->GetPosition();
	}

	coheed = coheed / (float)(size);
	averagePos = Vector3(Vector3(coheed).x,0,Vector3(coheed).z);

	int Xmin = 20;
	int	Xmax = 20;
	//int	Ymin = 1;
	//int	Ymax = 1;
	int	Zmin = 20;
	int	Zmax = 20;
	Vector3 v = Vector3(0.f,0.f,0.f);

	if(b->GetPosition().x < (coheed.x - Xmin))
		v.x = 400;
	if(b->GetPosition().x > (coheed.x + Xmax))
		v.x = -400;

	//if(b->GetPosition().y < (coheed.y - Ymin))
	//	v.y = 1;
	//if(b->GetPosition().position.y > (coheed.y + Ymax))
	//	v.y = -1;

	if(b->GetPosition().z < (coheed.z - Zmin))
		v.z = 400;
	if(b->GetPosition().z > (coheed.z + Zmax))
		v.z = -400;

	coheed = coheed + v;

	if(!kingPos)
		coheed = (coheed + flockPos)/2;
	else
		coheed = (coheed + bodyArr[0]->GetPosition())/2;

	coheed = coheed - b->GetPosition();	

	return coheed;
}