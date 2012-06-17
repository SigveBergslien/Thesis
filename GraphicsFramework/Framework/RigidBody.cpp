#include "RigidBody.h"

RigidBody::RigidBody(Vector3 pos, float mass){
	position = pos;
	if(mass>0){
		invMass = 1/mass;
	}
	else{
		invMass=0;
	}
	linearVelocity = Vector3(0,0,0);
	force = Vector3(0,0,0);
	orientation = Quaternion(1,1,1,1);
	orientation.Normalise();
	angularVelocity = Vector3(0,0,0);
	torque = Vector3(0,0,0);
	obj = NULL;

	//flags
	isSphere = false;
	isRotating = true;
	type=-1;
	lastCollision = 0;
	prePos = pos;
	meteor=false;
}

RigidBody::RigidBody(){
	position = Vector3(0,0,0);
	invMass = 1;
	linearVelocity = Vector3(0,0,0);
	force = Vector3(0,0,0);
	orientation = Quaternion(1,1,1,1);
	orientation.Normalise();
	angularVelocity = Vector3(0,0,0);
	torque = Vector3(0,0,0);
	obj = NULL;

	//flags
	isSphere = false;
	type = -1;
}

void RigidBody::SetTypeSphere(int r){
	obj = new BoundingSphere((float)r,position);
	isSphere = true;
	float tmp = 1.0f /((2.0f/5.0f)*(1/invMass)*r*r);
	invInertia = Matrix4::Scale(Vector3(tmp,tmp,tmp));
	if(!r){
		isRotating = false;
	}
}

void RigidBody::verletIntegrate(float dt){
	if(invMass){
		Vector3 a = (force*invMass*dt);
		Vector3 tmpPos = (position*2)-prePos + a * dt * dt;
		prePos = position;
		position = tmpPos;
		obj->updatePosition(position);
	}
}