#pragma once
#include "Vector3.h"
#include "RigidBody.h" 

class ClothSpring{
public:
	float springConstant;
	RigidBody* obj[2];
	float originalLenght;
	ClothSpring(){}
	ClothSpring(float sc,RigidBody* object1,RigidBody* object2, float ol){springConstant = sc; obj[0] = object1;obj[1] = object2; originalLenght = ol;}
	inline void initialize(float sc,RigidBody* object1,RigidBody* object2, float ol){springConstant = sc; obj[0] = object1;obj[1] = object2; originalLenght = ol;}
	void update(){
		Vector3 tmpSpring =  obj[1]->GetPosition()- obj[0]->GetPosition();
		float length = tmpSpring.Length();
		float displacement = length - originalLenght;
		Vector3 springNormal = tmpSpring /length;
		Vector3 fus = springNormal * ((displacement * springConstant)-(10*Vector3::Dot(springNormal,obj[0]->GetVelocity())));
		obj[0]->AddForce(fus);
		obj[1]->AddForce(-fus);
	}
};