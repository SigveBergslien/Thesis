#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/LoadingCamera.h"
#include "../Framework/HeightMap.h"
#include "../Framework/OBJMesh.h"
#include "../Framework/TextMesh.h"
#include <time.h>
class MenuRenderer : public OGLRenderer
{
public: 
	MenuRenderer(Window &parent);
	virtual ~MenuRenderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	void checkBounds(Vector2 mousePos);
	bool start;
	bool exitFlag;
	bool creditsFlag;
	bool continueGameFlag;
	

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
protected:
	int move;
	Vector2 quadPos;
	OCamera* camera;
	Light* light;
	float rot;
	clock_t timer;
	bool pressEnter;
	Font*	basicFont;	//A font! a basic one...
	OBJMesh* mesh;
	Mesh* quad2;
	Mesh* quad;
	Mesh* circle;
	Mesh* options;
	Mesh* newGame;
	Mesh* vines;
	Mesh* continueGame;
	Mesh* exitGame;
	Mesh* credits;
	Mesh* cursorQuad;
	Mesh* creditsPage;
	
	

	
};