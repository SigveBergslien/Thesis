#include "LoadingRenderer.h"

//basic loading screen using perspective proj
//changes from opening title to loading screen dependant on bool sent to it from graphics.cpp

LoadingRenderer::LoadingRenderer(Window& parent, bool LoadingScreen) : OGLRenderer(parent)
{
	
	camera = new OCamera(0.0f, 0.0f, Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f, 200, RAW_HEIGHT*HEIGHTMAP_Z));
	loadingScreen = LoadingScreen;
	
	quad = Mesh::GenerateQuad();
	quad2 = Mesh::GenerateQuad();
	webQuad = Mesh::GenerateQuad();
	numberQuad = Mesh::GenerateQuad();
	webQuad->SetTexture(SOIL_load_OGL_texture("../Textures/weblogo.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	currentShader = new Shader("../Shaders/cursorVertex.glsl", "../Shaders/cursorFragment.glsl");
	if (loadingScreen)
	{
	quad->SetTexture(SOIL_load_OGL_texture("../Textures/menuScreen.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad2->SetTexture(SOIL_load_OGL_texture("../Textures/arrows.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	}

	else
	{
		quad->SetTexture(SOIL_load_OGL_texture("../Textures/lava.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		
		quad2->SetTexture(SOIL_load_OGL_texture("../Textures/logoFire.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		
	}
	
	if (!currentShader->LinkProgram() || !quad2->GetTexture())
	{
		return;
	}
		
	
	light = new Light(Vector3((RAW_HEIGHT*HEIGHTMAP_X/ 2.0f), 500.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)), Vector4(1,1,1,1), (RAW_WIDTH*HEIGHTMAP_X) * 2.0f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float) width/ (float) height, 45.0f);
	
	
	glEnable(GL_DEPTH_TEST);
	init = true;
	ls = true;
	move = 0;
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);	
	rot = 0.0f;
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

LoadingRenderer::~LoadingRenderer(void)
{
	delete camera;
	delete light;	
	delete quad;
	delete quad2;
	
	delete numberQuad;
	
	
	
}

void LoadingRenderer::UpdateScene(float msec)
{
	if (loadingScreen) rot+= msec/2;
	else rot += msec/2;
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void LoadingRenderer::RenderScene()
{

	wglMakeCurrent(deviceContext, renderContext);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,1);
	glUseProgram(currentShader->GetProgram());
	
//	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	//glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();

	projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
		
	modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,0)) * Matrix4::Scale(Vector3((float)width/2,(float)height/2,1));
	UpdateShaderMatrices();
		
	SetShaderLight(*light);
	if (loadingScreen)
	quad->Draw();
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	if (loadingScreen)
		modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,0.5f)) * Matrix4::Scale(Vector3(60,60,1)) * Matrix4::Rotation(-rot,Vector3(0,0,1));
	else
	{
		if ((float)height + 1000 - rot > (float)height/2)
		{
			modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height + 1000 - rot,0.5f)) * Matrix4::Scale(Vector3(600,100,1));// * Matrix4::Rotation(-rot,Vector3(0,0,1));
		}
		else{
			if (rot > 2500)
			{
				modelMatrix = Matrix4::Translation(Vector3((float)width/2-(float)width/50,(float)height/2-200,0.9f)) * Matrix4::Scale(Vector3(200,50,1));
				UpdateShaderMatrices();
				webQuad->Draw();
			}
			modelMatrix = Matrix4::Translation(Vector3((float)width/2-(float)width/50,(float)height/2-50,0.9f)) * Matrix4::Scale(Vector3(200,200,1));
			UpdateShaderMatrices();
			
			modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,0.5f)) * Matrix4::Scale(Vector3(600,100,1));
			if (rot > 4000)			
				ls = false;			
		}
	}
	UpdateShaderMatrices();
	quad2->Draw();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float) width/ (float) height, 45.0f);
	UpdateShaderMatrices();
	glUseProgram(0);
	SwapBuffers();
}