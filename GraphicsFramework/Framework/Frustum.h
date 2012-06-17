#pragma once

#include "Plane.h"
#include "Matrix4.h"
#include "SceneNode.h"

class Matrix4;		// Compile the Mat4 class first , please !

class SceneNode;
class Frustum
{
public:
	Frustum(void) { };
	~Frustum(void) { };

	void		 FromMatrix(const Matrix4 &mvp);
	bool		InsideFrustum(SceneNode &n);
	bool		CubeInFrustum(Vector3 pos,float size);
protected :
	Plane planes[6];
};