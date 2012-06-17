#pragma once
#include "Mesh.h"
#include "RigidBody.h"
#include "ClothSpring.h"
#include "Spring.h"
#include "BoundingSphere.h"

class Cloth:public Mesh{
public:
	Cloth(int width, int height, int size,Vector3 corner,Vector3 corner2,Vector3 corner3,Vector3 corner4);
	~Cloth(){};
	void update(float msec);
	int GetNumVertices(){return numVertices;}
	BoundingSphere getBoundingSphere() const{return bounding;};
	RigidBody* GetBodies(){return bodies;}
	void AddForce(Vector3 force);
	void FusRoDah(Vector3 cam);
	void RebufferData();
private:
	BoundingSphere bounding;
	RigidBody *bodies;
	int numSprings;
	ClothSpring *springs;
	Spring staticSprings[4];
};