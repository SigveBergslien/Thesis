/*#include "Collision.h"
#include <iostream>

using namespace std;

Collision::Collision()
{
}


Collision::~Collision()
{
}

bool Collision::SphereSphereCollision(const SphereCol& s0, const SphereCol& s1, CollisionData* collision)
{
	const float distSq = LengthSq(s0.m_pos - s1.m_pos);

	const float sumRadius = (s0.m_radius + s1.m_radius);

	if(distSq < sumRadius * sumRadius)
	{
		if(collision)
		{
			collision->m_penetration = sumRadius - sqrtf(distSq);
			collision->m_normal = Normalize(s0.m_pos - s1.m_pos);
			collision->m_point = s0.m_pos - collision->m_normal * (s0.m_radius - collision->m_penetration * 0.5f);
		}
		return true;
	}

	return false;
}

bool Collision::SphereQuadCollision(const SphereCol& s0, const Vector3& mins, const Vector3& maxs, CollisionData* collision)
{
	const Vector3 sphereMins = Vector3(s0.m_pos.x - s0.m_radius, s0.m_pos.y - s0.m_radius, s0.m_pos.z - s0.m_radius);
	const Vector3 sphereMaxs = Vector3(s0.m_pos.x + s0.m_radius, s0.m_pos.y + s0.m_radius, s0.m_pos.z + s0.m_radius);

	bool hit = false;

	if(collision) collision->m_penetrationXYZ = Vector3(0.0f, 0.0f, 0.0f);

	if(sphereMins.x <= mins.x)
	{
		if(collision)
		{
			collision->m_penetrationXYZ.x = sphereMins.x - mins.x;
			collision->m_normal = Vector3(1.0f, 0.0f, 0.0f);
			collision->m_point = Vector3(mins.x, 0.0f, 0.0f);
		}
		hit = true;
	}
	else if(sphereMaxs.x >= maxs.x)
	{
		if(collision)
		{
			collision->m_penetrationXYZ.x = sphereMaxs.x - maxs.x;
			collision->m_normal = Vector3(-1.0f, 0.0f, 0.0f);
			collision->m_point = Vector3(maxs.x, 0.0f, 0.0f);
		}
		hit = true;
	}
		
	if(sphereMins.y <= mins.y)
	{
		if(collision)
		{
			collision->m_penetrationXYZ.y = sphereMins.y - mins.y;
			collision->m_normal += Vector3(0.0f, 1.0f, 0.0f);
			collision->m_point += Vector3(0.0f, mins.y, 0.0f);
		}
		hit = true;
	}
	else if(sphereMaxs.y >= maxs.y)
	{
		if(collision)
		{
			collision->m_penetrationXYZ.y = sphereMaxs.y - maxs.y;
			collision->m_normal += Vector3(0.0f, -1.0f, 0.0f);
			collision->m_point += Vector3(0.0f, maxs.y, 0.0f);
		}
		hit = true;
	}

	if(sphereMins.z <= mins.z)
	{
		if(collision)
		{
			collision->m_penetrationXYZ.z = sphereMins.z - mins.z;
			collision->m_normal += Vector3(0.0f, 0.0f, 1.0f);
			collision->m_point += Vector3(0.0f, 0.0f, mins.z);
		}
		hit = true;
	}
	else if(sphereMaxs.z >= maxs.z)
	{
		if(collision)
		{
			collision->m_penetrationXYZ.z = sphereMaxs.z - maxs.z;
			collision->m_normal += Vector3(0.0f, 0.0f, -1.0f);
			collision->m_point += Vector3(0.0f, 0.0f, maxs.z);
		}
		hit = true;
	}

	if(hit && collision)
	{
		collision->m_normal.Normalise();
		collision->m_penetration = Length(collision->m_penetrationXYZ);
		//collision->m_point = s0.m_pos - collision->m_normal * (s0.m_radius - collision->m_penetration * 0.5f);
	}

	return hit;
}

void Collision::AddCollisionImpulse(Body& b1, const Vector3& hitPoint, const Vector3& normal, float penetration, const Vector3& penetrationXYZ, Body& b2)
{
	float invMass1 = b1.GetMass() > 1000.0f ? 0.0f : (1.0f / b1.GetMass());
	float invMass2 = b2.GetMass() > 1000.0f ? 0.0f : (1.0f / b2.GetMass());

	invMass1 = (!b1.GetAwake()) ? 0.0f : invMass1;
	invMass2 = (!b2.GetAwake()) ? 0.0f : invMass2;

	const Matrix4 worldInvInertia1 = b1.GetInvInertia();
	const Matrix4 worldInvInertia2 = b2.GetInvInertia();

	if((invMass1 + invMass2) == 0.0) return;

	Vector3 r1 = hitPoint - b1.GetPosition();
	Vector3 r2 = hitPoint - b2.GetPosition();

	Vector3 v1 = b1.GetVelocity() + Vector3::Cross(b1.GetVelocityAngular(), r1);
	Vector3 v2 = b2.GetVelocity() + Vector3::Cross(b2.GetVelocityAngular(), r2);

	Vector3 dv = v1 - v2;

	float relativeMovement = -Vector3::Dot(dv, normal);

	if(relativeMovement < 0.0f)
	{
		return;
	}
	else
	{
		// Normal Impulse

		float e = 0.5f;

		float normDiv = Vector3::Dot(normal, normal) * ((invMass1 + invMass2) + Vector3::Dot(normal, Vector3::Cross((worldInvInertia1 * Vector3::Cross(r1, normal)), r1) + Vector3::Cross((worldInvInertia2 * Vector3::Cross(r2, normal)), r2)));

		float jn = -1 * (1 + e) * Vector3::Dot(dv, normal) / normDiv;

		jn = jn + (penetration * 1.5f);

		b1.SetVelocity(b1.GetVelocity() + normal * invMass1 * jn);
		b1.SetVelocityAngular(b1.GetVelocityAngular() + (worldInvInertia1 * (Vector3::Cross(r1, normal * jn))));

		b2.SetVelocity(b2.GetVelocity() - normal * invMass2 * jn);
		b2.SetVelocityAngular(b2.GetVelocityAngular() - (worldInvInertia2 * (Vector3::Cross(r2, normal * jn))));

		// Tangent

		Vector3 tangent = Vector3(0, 0, 0);
		tangent = dv - (normal * Vector3::Dot(dv, normal));
		tangent.Normalise();

		float tangDiv = invMass1 + invMass2 + Vector3::Dot(tangent, (Vector3::Cross((b1.GetInvInertia() * Vector3::Cross(r1, tangent)), r1) + Vector3::Cross((b2.GetInvInertia() * Vector3::Cross(r2, tangent)), r2)));

		float jt = -1 * Vector3::Dot(dv, tangent) / tangDiv;

		b1.SetVelocity(b1.GetVelocity() + tangent * jt * invMass1);
		b1.SetVelocityAngular(b1.GetVelocityAngular() + worldInvInertia1 * Vector3::Cross(r1, tangent * jt));

		b2.SetVelocity(b2.GetVelocity() - tangent * jt * invMass2);
		b2.SetVelocityAngular(b2.GetVelocityAngular() + worldInvInertia2 * Vector3::Cross(r2, tangent * jt));
	}
}

void Collision::AddCollisionImpulse(Body& b1, const Vector3& hitPoint, const Vector3& normal, float penetration, bool angular)
{
	float invMass1 = b1.GetMass() > 1000.0f ? 0.0f : (1.0f / b1.GetMass());

	invMass1 = (!b1.GetAwake()) ? 0.0f : invMass1;

	const Matrix4 worldInvInertia1 = b1.GetInvInertia();

	if(invMass1 == 0.0) return;

	Vector3 r1 = hitPoint - b1.GetPosition();

	Vector3 v1 = b1.GetVelocity() + Vector3::Cross(b1.GetVelocityAngular(), r1);

	Vector3 dv = v1;

	float relativeMovement = -Vector3::Dot(dv, normal);

	if(relativeMovement < 0.0f)
	{
		return;
	}
	else
	{
		// Normal Impulse

		float e = 0.9f;

		float normDiv = Vector3::Dot(normal, normal) * ((invMass1) + Vector3::Dot(normal, Vector3::Cross((worldInvInertia1 * Vector3::Cross(r1, normal)), r1)));

		float jn = -1 * (1 + e) * Vector3::Dot(dv, normal) / normDiv;

		jn = jn + (penetration * 5.0f);

		b1.SetVelocity(b1.GetVelocity() + normal * invMass1 * jn);

		if(angular)	b1.SetVelocityAngular(b1.GetVelocityAngular() + (worldInvInertia1 * (Vector3::Cross(r1, normal * jn))));

		// Tangent

		Vector3 tangent = Vector3(0, 0, 0);
		tangent = dv - (normal * Vector3::Dot(dv, normal));
		tangent.Normalise();

		float tangDiv = invMass1 + Vector3::Dot(tangent, (Vector3::Cross((b1.GetInvInertia() * Vector3::Cross(r1, tangent)), r1)));

		float jt = -1 * Vector3::Dot(dv, tangent) / tangDiv;

		if(jt > 0.001) 
		{
			b1.SetVelocity(b1.GetVelocity() + tangent * (jt * 1250) * invMass1);
			if(angular)b1.SetVelocityAngular(b1.GetVelocityAngular() + worldInvInertia1 * Vector3::Cross(r1, tangent * jt));
		}
	}
}

float Collision::LengthSq(Vector3 vec)
{
	return(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float Collision::Length(Vector3 vec)
{
	return(sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
}

Vector3 Collision::Normalize(Vector3 vec)
{
	Vector3 tmp = vec;
	tmp.Normalise();
	return tmp;
}
 */