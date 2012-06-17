#include "CollisionResponse.h"

void CollisionResponse::Collide(CollisionData* col, RigidBody& a, RigidBody& b){
	Vector3 tmpForce = col->normal * col->penetration * 250;
	if(tmpForce.y!=0.0f){
		tmpForce.y=0.0f;
	}
	a.AddForce(tmpForce);
	b.AddForce(-tmpForce);
}

void CollisionResponse::Collide(CollisionData* col, RigidBody& a){
	Vector3 tmpForce = col->normal * col->penetration * 250;
	if(tmpForce.y!=0.0f){
		tmpForce.y=0.0f;
	}
	a.AddForce(-tmpForce);
}

void CollisionResponse::CollideI(CollisionData* col, RigidBody& a, RigidBody& b){
	Vector3 r0 = col->point  - a.GetPosition();
	Vector3 r1 = col->point - b.GetPosition();

	Vector3 v0 = a.GetVelocity() + Vector3::Cross(a.GetAngularVelocity() , r0);
	Vector3 v1 = b.GetVelocity() + Vector3::Cross(b.GetAngularVelocity(),r1);

	Vector3 dv = v0 - v1;
	float relativeMovement = -Vector3::Dot(dv,col->normal);
	if(relativeMovement <0.0f){
		return;
	}
	{
		float e = 0.9f;
		float normDiv = Vector3::Dot(col->normal,col->normal) * ((a.GetInvMass()+b.GetInvMass())+Vector3::Dot(col->normal,
			Vector3::Cross(a.CreateWorldII()*Vector3::Cross(r0,col->normal),r0)+
			Vector3::Cross(b.CreateWorldII()*Vector3::Cross(r1,col->normal),r1)));

		float jn = -1*(1+e)*Vector3::Dot(dv,col->normal)/normDiv;
		jn += (float)col->penetration*1.5f;

		a.AddVelocity( col->normal * a.GetInvMass() * jn);
		a.AddAngularVelocity(a.CreateWorldII()*Vector3::Cross(r0,col->normal*jn));

		b.AddVelocity( -(col->normal * b.GetInvMass() * jn));
		b.AddAngularVelocity(-(b.CreateWorldII()*Vector3::Cross(r1,col->normal*jn)));
	}
	{
		Vector3 tangent = Vector3(0,0,0);
		tangent = dv- (col->normal*Vector3::Dot(dv,col->normal));
		tangent.Normalise();

		float tangDiv = a.GetInvMass() + b.GetInvMass()+ Vector3::Dot(tangent, Vector3::Cross(a.GetInvInertia()*Vector3::Cross(r0,tangent),r0)+
			Vector3::Cross(b.GetInvInertia()*Vector3::Cross(r1,tangent),r1));

		float jt = -1 * Vector3::Dot(dv,tangent)/tangDiv;

		a.AddVelocity((tangent*a.GetInvMass()*jt)/20);
		a.AddAngularVelocity(a.CreateWorldII()*Vector3::Cross(r0,tangent*jt));

		a.AddVelocity(-(tangent*a.GetInvMass()*jt)/20);
		a.AddAngularVelocity(-(a.CreateWorldII()*Vector3::Cross(r1,tangent*jt)));
	}
}

void CollisionResponse::CollideI(CollisionData* col, RigidBody& a){
		Vector3 r0 = col->point  - a.GetPosition();

	Vector3 v0 = a.GetVelocity() + Vector3::Cross(a.GetAngularVelocity() , r0);

	float relativeMovement = -Vector3::Dot(v0,col->normal);
	if(relativeMovement <1.0f){
		return;
	}
	{
		float e = 0.9f;
		float normDiv = Vector3::Dot(col->normal,col->normal) * (a.GetInvMass()+Vector3::Dot(col->normal,
			Vector3::Cross(a.CreateWorldII()*Vector3::Cross(r0,col->normal),r0)));

		float jn = -1*(1+e)*Vector3::Dot(v0,col->normal)/normDiv;
		jn += (float)col->penetration*2.5f;

		a.AddVelocity( col->normal * a.GetInvMass() * jn);
		a.AddAngularVelocity(a.CreateWorldII()*Vector3::Cross(r0,col->normal*jn));
	}
	{
		Vector3 tangent = Vector3(0,0,0);
		tangent = v0- (col->normal*Vector3::Dot(v0,col->normal));
		tangent.Normalise();

		float tangDiv = a.GetInvMass() + Vector3::Dot(tangent, Vector3::Cross(a.GetInvInertia()*Vector3::Cross(r0,tangent),r0));

		float jt = -1 * Vector3::Dot(v0,tangent)/tangDiv;

		a.AddVelocity(tangent*a.GetInvMass()*jt);
		a.AddAngularVelocity(a.CreateWorldII()*Vector3::Cross(r0,tangent*jt));
	}
}

void CollisionResponse::CollideIC(CollisionData* col, RigidBody& a, RigidBody& b){
	//Vector3 r0 = col->point  - a.GetPosition();
	//Vector3 r1 = col->point - b.GetPosition();

	//Vector3 v0 = a.GetVelocity() + Vector3::Cross(a.GetAngularVelocity() , r0);
	//Vector3 v1 = b.GetVelocity() + Vector3::Cross(b.GetAngularVelocity(),r1);

	//Vector3 dv = v0 - v1;
	//float relativeMovement = -Vector3::Dot(dv,col->normal);
	//if(relativeMovement <0.0f){
	//	return;
	//}
	//{
	//	float e = 0.9f;
	//	float normDiv = Vector3::Dot(col->normal,col->normal) * ((a.GetInvMass()+b.GetInvMass())+Vector3::Dot(col->normal,
	//		Vector3::Cross(a.CreateWorldII()*Vector3::Cross(r0,col->normal),r0)));

	//	float jn = -1*(1+e)*Vector3::Dot(dv,col->normal)/normDiv;
	//	jn += (float)col->penetration*1.5f;

	//	//a.AddVelocity( col->normal * a.GetInvMass() * jn);
	//	//a.AddAngularVelocity(a.CreateWorldII()*Vector3::Cross(r0,col->normal*jn));

	//	b.AddVelocity( -(col->normal * b.GetInvMass() * jn)*5);
	//}
	Vector3 tmpForce = col->normal * col->penetration * 15000;
	b.AddForce(-tmpForce);
}