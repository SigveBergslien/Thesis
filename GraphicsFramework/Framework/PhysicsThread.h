#pragma once
#include "Thread.h"
#include "RigidBody.h"
#include "CollisionData.h"
#include "CollisionDetection.h"
#include "CollisionResponse.h"
#include "Vector3.h"
#include "Flock.h"
#include "Mutex_class.h"
#include "HeightMap.h"
#include "Cloth.h"
#include "SceneNode.h"
#include "GameObjectTypes.h"

class Frustum;


#include "Frustum.h"
#include "Octree.h"

#define NUM_BODIES 512

class SceneNode;

class Octree;
class PhysicsThread : public Thread{
public:
	//threading functions
	~PhysicsThread();
	virtual void run();
	void stop();

	void initialize(Window &parent);
	void setHeightMap(HeightMap* hmap){map=hmap;}
	void setFrustum(Frustum &frust){frustum = &frust;}

	int addRigidBody(Vector3 pos, float radius, float mass, GameObject* obj);
	void addToFlock();
	RigidBody* getBody(int i);
	void setVelocity(int i,Vector3 velo);
	void collisions();
	void testType(int i, int j);

	void Integrate(float msec);

	inline void SetCape(Cloth* _cape){cape = _cape;}
	void updateCloth(Cloth* cloth, float msec);

	inline void toggleClothUpdate(){clothUpdateing=!clothUpdateing;}
	/*
	Manually set the postion of the king. Ignoring physics
	*/
	inline void setPositionKing(Vector3 pos)
	{
		mut.lock_mutex();
		kingPos=pos;
		mut.unlock_mutex();
	}
	inline void togglePause(){pause = !pause;}
	void resetPhysics();
	void setResetPhysics();
	void setFlockPos(Vector3 &pos);
	void resetFlockPos()
	{ 
		flockMut.lock_mutex_wait();
		flock->resetFlock();
		flockMut.unlock_mutex();
	}
	void				RecreateOctree();
	Octree*				GetOctree() { return rootOctTree;}
	inline int			getNumberInGroup(){return numberInGroup;}
	inline Vector3		getFlockPos(){return flockPos;}
private:
	bool running;

	//bodies
	RigidBody** bodies;
	RigidBody** tmpBodies; //copy for adding
	RigidBody** flockBodies;//copy of bodies in a flock
	GameObject** gameObj;
	int numberOfBodies;

	HeightMap* map;
	Window* w;

	CollisionDetection colDet;
	CollisionResponse colRes;

	Flock* flock;
	Vector3 tmpVelo[NUM_BODIES];
	Mutex_class mut;
	Mutex_class flockMut;
	Cloth* cape;
	Vector3 kingPos;

	//flags
	bool pause;
	bool addFlock;
	bool clothUpdateing;
	bool reset;

	//Octree
	Octree *rootOctTree;
	Frustum* frustum;
	int countTestOctree;
	int octreeResetTimer;

	int numberInGroup;
	Vector3 flockPos;
	Vector3 meteorDirection;
};