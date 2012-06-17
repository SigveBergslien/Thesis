#include "CollisionDetection.h"

bool CollisionDetection::Collision(RigidBody& a, RigidBody& b,CollisionData* colDat)
{
	if(a.IsSphere()&&b.IsSphere())
	{
		return Collision((BoundingSphere*)a.getBoundingObject(),(BoundingSphere*)b.getBoundingObject(),colDat);
	}
	return false;
}
bool CollisionDetection::Collision(RigidBody& a, const BoundingTriangle& b, CollisionData* colDat)
{
	Vector3 tmp = a.GetPosition();
	
	if(a.IsSphere())
	{
		return Collision((BoundingSphere*)a.getBoundingObject(),b,colDat);
	}
	return false;
}

bool CollisionDetection::Collision(BoundingSphere* a, BoundingSphere* b,CollisionData* colDat)
{
	const float distSq = (a->position- b->position).SqrLength();

	const float sumRadii = a->radius + b->radius;
	if(distSq < sumRadii*sumRadii)
	{
		colDat->penetration = sumRadii - sqrt(distSq);
		colDat->normal = (a->position - b->position);
		colDat->normal.Normalise();
		colDat->point = a->position- colDat->normal*(a->radius - colDat->penetration*0.5f);
		return true;
	}
	return false;
}

bool CollisionDetection::Collision(BoundingSphere* a,const BoundingTriangle& b, CollisionData* colDat)
{
	Vector3 planeN = Vector3::Cross((b.vertices[0] - b.vertices[1]),(b.vertices[0] - b.vertices[2]));
	planeN.Normalise();

	Vector3 pos = a->position;
	float distance =  Vector3::Dot(planeN,pos) - Vector3::Dot(b.vertices[0],planeN);
		float pen = distance - a->radius;
		if(pen < 0)
		{
			Vector3 point = a->position- planeN*(a->radius + ((pen)));
			Vector3 vecA = b.vertices[0] - point;
			float lenghtA = vecA.Length();
			Vector3 vecB = b.vertices[1] - point;
			float lenghtB = vecB.Length();
			Vector3 vecC = b.vertices[2] - point;
			float lenghtC = vecC.Length();
			float angleA = acos(Vector3::Dot(vecA,vecB)/(lenghtA*lenghtB));
			float angleB = acos(Vector3::Dot(vecB,vecC)/(lenghtB*lenghtC));
			float angleC = acos(Vector3::Dot(vecC,vecA)/(lenghtC*lenghtA));
			if(angleA+angleB+angleC >6.25||!lenghtA||!lenghtB||lenghtC){
				colDat->penetration = -pen;
				colDat->normal = planeN;
				colDat->point = point;
				return true;
		}
	}
	return false;
}

void CollisionDetection::Collision(RigidBody& a, Cloth* b)
{
	if(a.IsSphere())
	{
		RigidBody* bodies = b->GetBodies();
		CollisionResponse colRes;
		const float distSq = (a.getBoundingObject()->position - b->getBoundingSphere().position).Length();
		BoundingSphere* s = (BoundingSphere*)a.getBoundingObject();
		const float sumRadii = s->radius + b->getBoundingSphere().radius;
		//if(distSq < sumRadii){
			for(int i =0; i< b->GetNumVertices();++i){
				(a.getBoundingObject()->position- bodies[i].getBoundingObject()->position).SqrLength();
				CollisionData* colDat = new CollisionData();
				if(Collision((BoundingSphere*)a.getBoundingObject(),(BoundingSphere*)bodies[i].getBoundingObject(),colDat))
				{
					colRes.CollideIC(colDat, a, bodies[i]);
				}
				delete colDat;
			}
		//}
	}
}
