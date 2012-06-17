#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/LoadingCamera.h"
#include "../Framework/HeightMap.h"
#include "../Framework/OBJMesh.h"


class LoadingRenderer : public OGLRenderer
{
public: 
	LoadingRenderer(Window &parent, bool LoadingScreen);
	virtual ~LoadingRenderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	bool getLoadingScreen() {return loadingScreen;}
	bool getLoadingStatus() {return ls;}

protected:
	int move;
	Mesh* heightMap;
	Mesh* numberQuad;
	Mesh* webQuad;
	OCamera* camera;
	Light* light;
	float rot;
	bool loadingScreen;
	bool ls;
	OBJMesh* mesh;
	Mesh* quad2;
	Mesh* quad;
	
	
};