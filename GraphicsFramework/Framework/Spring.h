#pragma once
#include "Vector3.h"
#include "RigidBody.h" 

class Spring{
public:
	float springConstant;
	Vector3 endPoint;
	RigidBody* obj;
	float originalLenght;
	Spring(){}
	Spring(float sc,Vector3 end,RigidBody* object, float ol){springConstant = sc; endPoint = end;obj = object; originalLenght = ol;}
	void update(){
		Vector3 tmpSpring = endPoint - obj->GetPosition();
		float length = tmpSpring.Length();
		if(length){
			float displacement = length - originalLenght;
			Vector3 springNormal = tmpSpring /length;
			Vector3 fus = springNormal * ((displacement * springConstant)-(70*Vector3::Dot(springNormal,obj->GetVelocity())));
			obj->AddForce(fus);
		}
	}
};