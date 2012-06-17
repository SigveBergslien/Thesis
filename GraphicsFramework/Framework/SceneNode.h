#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include "PhysicsThread.h"
#include "GameObjectTypes.h"

class PhysicsThread;
class SceneNode
{
public:
	SceneNode(Mesh *m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4 &matrix)		{ transform = matrix; }
	const Matrix4 & GetTransform() const			{ return transform; }

	void SetWorldTransform(const Matrix4 &matrix)	{ worldTransform = matrix; }
	Matrix4 GetWorldTransform() const				{ return worldTransform; }

	Vector4 GetColour() const						{ return colour; }
	void SetColour(Vector4 c)						{ colour = c; }

	Vector3 GetModelScale() const					{ return modelScale; }
	void SetModelScale(Vector3 s)					{ modelScale = s; }

	Mesh * GetMesh() const							{ return mesh; }
	void SetMesh(Mesh *m)							{ mesh = m; }

	void SetRigidBody(float radius, PhysicsThread* _engine, float mass);
	int	 GetRigidBody()								{ return rigidBody; } //if 0 no rigidbody, if not zero its the id

	void SetDirection(Vector3 dir) {direction = dir;}
	Vector3 GetDirection() {return direction;}
	float GetBoundingRadius() const					{ return boundingRadius; }
	void SetBoundingRadius(float f)					{ boundingRadius = f; }

	float GetCameraDistance() const					{ return distanceFromCamera; }
	void SetCameraDistance(float f)					{ distanceFromCamera = f; }

	bool HasChildren()								{	
														if(children.size() >0) 
															return true;
														else
															return false;
													}
	
	Vector3 GetVelocity()							{ return velocity; }

	SceneNode * GetParent()							{ return parent; }

	Vector3 GetTargetPosition();

	std::vector<SceneNode *>::const_iterator GetChildIteratorStart()	{ return children.begin(); }
	std::vector<SceneNode *>::const_iterator GetChildIteratorEnd()		{ return children.end(); }

	static bool CompareByCameraDistance(SceneNode *a, SceneNode *b)		{ return (a->distanceFromCamera < b->distanceFromCamera) ? true : false; }

	void AddChild(SceneNode *s);
	static void RemoveChild(SceneNode *s);

	virtual void Update(float msec);
	void updatePosition(float msec);

	void addVelocity(Vector3 vel, int i);

	void setPath(Vector3* p, int i)
	{
		if(p)
		{
			pathV= p;
			pathSize=i;
			movingTowards=1;
			needPath = false;
		}
	}
	int getInt(){return movingTowards;}
	void setNeedPath() { needPath = true; }
	bool getNeedPath() { return needPath; }
	void clearPath();
	inline void setFPS(bool b) { FPS = b; }

	GameObject objData;

	void gamePlayUpdate(float msec);

protected:
	SceneNode	*parent;
	Mesh		*mesh;
	Matrix4		worldTransform;
	Matrix4		transform;
	Vector3		modelScale;
	Vector4		colour;
	std::vector<SceneNode *> children;

	int size;
	Vector3 *path;

	int rigidBody;
	float boundingRadius;
	float distanceFromCamera;

	PhysicsThread* engine;

	int pathSize;
	Vector3* pathV;
	int movingTowards;
	bool needPath;
	bool FPS;
	Vector3 velocity;
	Vector3 lastVelocity;
	Vector3 direction;
};