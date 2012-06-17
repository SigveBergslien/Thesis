#ifndef _COLLISION_H_
#define _COLLISION_H_

#include <vector>
#include <list>
#include "Vector3.h"
#include "Matrix4.h"
//#include "Body.h"

class SphereCol
{
	public:

		SphereCol() {};
		SphereCol(const Vector3& p, float r) { m_pos = p; m_radius = r; }
		~SphereCol() {};
		
		Vector3 m_pos;
		float m_radius;
};

class CollisionData
{
	public:
		
		Vector3 m_point;
		Vector3 m_normal;
		float m_penetration;
		Vector3 m_penetrationXYZ;
};

/*class Collision
{
	public:

		Collision();
		~Collision();

		static bool SphereSphereCollision(const SphereCol& s0, const SphereCol& s1, CollisionData* collision = NULL);
		static bool SphereQuadCollision(const SphereCol& s0, const Vector3& mins, const Vector3& maxs, CollisionData* collision = NULL);
		static void AddCollisionImpulse(Body& b1, const Vector3& hitPoint, const Vector3& normal, float penetration, const Vector3& penetrationXYZ, Body& b2);
		static void AddCollisionImpulse(Body& b1, const Vector3& hitPoint, const Vector3& normal, float penetration, bool angular);

	protected:
		static float LengthSq(Vector3 vec);
		static float Length(Vector3 vec);
		static Vector3 Normalize(Vector3 vec);

	private:
}; */

#endif