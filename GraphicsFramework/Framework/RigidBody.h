#pragma once

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "BoundingSphere.h"


class RigidBody{
private:
	Vector3 position;
	Vector3 linearVelocity;
	Vector3 force;
	float invMass;

	Quaternion orientation;
	Vector3 angularVelocity;
	Vector3 torque;
	Matrix4 invInertia;

	BoundingObject* obj;

	//flags
	bool isSphere;
	bool isRotating;

	int type;

	Vector3 prePos;
	bool meteor;
public:
	RigidBody(Vector3 pos, float mass);
	RigidBody();
	~RigidBody(){if(obj)delete obj;}

	void Integrate(float dt){
		if(invMass){
			linearVelocity = linearVelocity *(1.0f - (0.03f*(dt*1000/20.0f)));
			linearVelocity += (force*invMass*dt);
			position +=linearVelocity *dt;
			force = Vector3(0,0,0);

			/*if(isRotating){
				Matrix4 worldInvInertia = CreateWorldII();
				angularVelocity = angularVelocity*0.98f;
				angularVelocity += worldInvInertia*torque*dt;
				orientation = orientation + Quaternion(angularVelocity.x*0.5f*dt,angularVelocity.y*0.5f*dt,angularVelocity.z*0.5f*dt,0.0f) * orientation;
				orientation.Normalise();
				torque = Vector3(0,0,0);
			}*/
			obj->updatePosition(position);
		}
	}

	void verletIntegrate(float dt);

	inline Matrix4 CreateWorldII(){
		/*Matrix4 orientationMatrix = orientation.ToMatrix();
		Matrix4 inverseWorldInertiaMatrix = Matrix4::Transpose(orientationMatrix) * invInertia * orientationMatrix;
		return inverseWorldInertiaMatrix;*/
		return Matrix4::Scale(Vector3(1,1,1));
	}
	inline void AddForce(Vector3 f){
		force += f;
	}
	inline void reset(Vector3 pos){
		position = pos;
		linearVelocity = Vector3(0,0,0);
		angularVelocity =  Vector3(0,0,0);
	}
	inline Vector3 GetPosition(){return position;}
	inline float GetMass(){return 1/invMass;}
	inline float GetInvMass(){return invMass;}
	inline Vector3 GetVelocity(){return linearVelocity;}
	inline Vector3 GetAngularVelocity(){return angularVelocity;}
	inline void AddVelocity(Vector3 v){linearVelocity += v;}
	inline void AddAngularVelocity(Vector3 v){angularVelocity += v;}
	inline Matrix4 GetInvInertia(){return invInertia;}
	inline Matrix4 GetOrientation(){//return orientation.ToMatrix();}
		return Matrix4::Scale(Vector3(1,1,1));}
	inline void SetPosition(Vector3 pos){position = pos;obj->position = pos;}
	inline void resetVelocity(){linearVelocity = Vector3(0,0,0);}
	inline void setRadius(float r){BoundingSphere* tmp = (BoundingSphere*)obj; tmp->radius = r; }
	void SetTypeSphere(int r);
	bool IsSphere(){return isSphere;}
	BoundingObject* getBoundingObject(){return obj;}
	void SetMass(float mass){
		if(mass){
			invMass = 1/mass;
		}
		else{
			invMass =0;
		}
	}
	void SetType(int t){type=t;}
	int getType(){return type;}

	void SetColType(int t){lastCollision=t;}
	int getColType(){return lastCollision;}

	void setMeteor(){meteor = true;}

	//0 == un initialised
	//1 == with pooka;
	//2 == with predator;
	//3 == with object
	//4 == with heightmap
	int lastCollision;
};