#pragma once

#include "Vector3.h"
#include "Frustum.h"
#include "DebugDraw3D.h"
#include "RigidBody.h"
#include <vector>

#define MAX_OBJECTS	75	//The maximum amount of objects in node
#define MAX_DEPTH 6		//The maxium depth of tree
#define MIN_NODES 15		//the minium objects in node 


//0 TOP_LEFT_FRONT, 1 TOP_LEFT_BACK, 2 TOP_RIGHT_BACK, 3 TOP_RIGHT_FRONT 
//4 BOTTOM_LEFT_FRONT, 5 BOTTOM_LEFT_BACK, 6 BOTTOM_RIGHT_BACK, 7 BOTTOM_RIGHT_FRONT
enum OctreeNodes 
{ 
	TOP_LEFT_FRONT,		
	TOP_LEFT_BACK,			
	TOP_RIGHT_BACK,			 
	TOP_RIGHT_FRONT, 
	BOTTOM_LEFT_FRONT, 
	BOTTOM_LEFT_BACK, 
	BOTTOM_RIGHT_BACK, 
	BOTTOM_RIGHT_FRONT 
}; 

// This is our octree class 
class Frustum;
class Octree 
{ 
public: 
	Octree(); 
	~Octree(); 

	Vector3		GetCenter()		{return center;} 
	int GetObjectCount()		{return objectCount;} 
	float GetDimension()		{return dimensions;} 
	bool IsSubDivided()			{return subDivided;} 
	
	// This gets the initial width, height and depth for the whole scene 
	void SetSceneDimensions(Vector3 minCorner, Vector3 maxCorner);
	
	// This takes in the previous nodes center, width and which node ID that will be subdivided 
	Vector3 GetNewNodeCenter(Vector3 center, float width, int nodeID); 
	
	// This subdivides a node depending on the objects and node dimensions
	void CreateNode(RigidBody** bodies, int numberOfObjects, Vector3 cent, float dim); 
	
	// This cleans up the new subdivided node creation process
	void CreateNewNode(RigidBody** bodies, std::vector<bool> pList, int numberOfObjects, Vector3 center,float dimension, int objCount, int nodeID); 
	
	// Once we are finished subdividing we need to assign the vertices to the end node. 
	void AssignObjectsToNode(RigidBody** bodies, int numberOfObj);
	void DrawDebugCube(Octree *node);
 
	// This goes through each of the nodes and then draws the end nodes vertices. 
	// This function should be called by starting with the root node. 
	void DrawOctree(Octree *pNode, Frustum *frameFrustum); 
	void DestroyOctree(); 
	
private: 
	void InitOctree(); 
	bool subDivided; 
	float dimensions; 
	int objectCount; 
	Vector3 center; 

	// This stores the objects that should be drawn with this node 
	//std::vector<Vector3> *objects;
	RigidBody** bodies;
	Octree *childNodes[8];	 
}; 