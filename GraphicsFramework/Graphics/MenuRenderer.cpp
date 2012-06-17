#include "MenuRenderer.h"

//basic loading screen using perspective proj

MenuRenderer::MenuRenderer(Window& parent) : OGLRenderer(parent)
{	
	camera = new OCamera(0.0f, 0.0f, Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f, 200, RAW_HEIGHT*HEIGHTMAP_Z));
	circle = Mesh::GenerateQuad();
	quad = Mesh::GenerateQuad();
	quad2 = Mesh::GenerateQuad();
	credits = Mesh::GenerateQuad();
	options = Mesh::GenerateQuad();
	newGame = Mesh::GenerateQuad();
	continueGame = Mesh::GenerateQuad();
	exitGame = Mesh::GenerateQuad();
	cursorQuad = Mesh::GenerateQuad();
	creditsPage = Mesh::GenerateQuad();
	currentShader = new Shader("../Shaders/cursorVertex.glsl", "../Shaders/cursorFragment.glsl");
	
	quad->SetTexture(SOIL_load_OGL_texture("../Textures/menuScreen.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad2->SetTexture(SOIL_load_OGL_texture("../Textures/menuScreen2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	options->SetTexture(SOIL_load_OGL_texture("../Textures/Options.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	newGame->SetTexture(SOIL_load_OGL_texture("../Textures/newGame.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	continueGame->SetTexture(SOIL_load_OGL_texture("../Textures/continueGame.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	exitGame->SetTexture(SOIL_load_OGL_texture("../Textures/exitGame.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	credits->SetTexture(SOIL_load_OGL_texture("../Textures/credits.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cursorQuad->SetTexture(SOIL_load_OGL_texture("../Textures/select.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	circle->SetTexture(SOIL_load_OGL_texture("../Textures/creature.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	creditsPage->SetTexture(SOIL_load_OGL_texture("../Textures/creditsPage.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	
	if (!currentShader->LinkProgram() || !quad2->GetTexture())
	{
		return;
	}
	
	light = new Light(Vector3((RAW_HEIGHT*HEIGHTMAP_X/ 2.0f), 500.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)), Vector4(1,1,1,1), (RAW_WIDTH*HEIGHTMAP_X) * 2.0f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float) width/ (float) height, 45.0f);
	basicFont = new Font(SOIL_load_OGL_texture("../Textures/tahoma.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

	quadPos = Vector2((float)width/2,(float)200);
	glEnable(GL_DEPTH_TEST);
	init = true;
	
	move = 0;
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);	
	rot = 0.0f;
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	start = false;
	continueGameFlag = false;
	exitFlag = false;
	creditsFlag = false;
	timer = time(0);
	pressEnter = true;
}

MenuRenderer::~MenuRenderer(void)
{
	delete camera;
	delete light;	
	delete quad;
	delete quad2;
	delete credits;
	delete creditsPage;
	delete options;
	delete newGame ;
	delete continueGame ;
	delete exitGame;
	delete cursorQuad ;
	delete basicFont;
}

void MenuRenderer::UpdateScene(float msec)
{
	rot += msec/2;
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RETURN)) pressEnter = false;
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_ESCAPE)) creditsFlag = false;
	
}

void MenuRenderer::RenderScene()
{	
	wglMakeCurrent(deviceContext, renderContext);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,1);
	glUseProgram(currentShader->GetProgram());
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();

	projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	
	modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,-0.99f)) * Matrix4::Scale(Vector3((float)width/2,(float)height/2,1));
	UpdateShaderMatrices();
		
	SetShaderLight(*light);
	if (pressEnter)	
	{
		quad->Draw();
		modelMatrix.ToIdentity();
		viewMatrix.ToIdentity();	
		
		UpdateShaderMatrices();
		//quad2->Draw();
	
			if (difftime(time(0),timer) >= 2)
			{
				DrawText("PRESS ENTER", Vector3((float)width/2.5f,(float)height/2.0f,0.5f),32.0f);
				if (difftime(time(0),timer) >= 4)
					timer = time(0);
			}

	}
	else
	{
		if (creditsFlag)
		{
			glDisable(GL_BLEND);

			quad2->Draw();
			modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,0.99f)) * Matrix4::Scale(Vector3((float)width/2,(float)height/2,1));
			UpdateShaderMatrices();
			creditsPage->Draw();
		
		}

		if (!creditsFlag)
		{
	
			glDisable(GL_BLEND);

			quad2->Draw();

			modelMatrix.ToIdentity();
			modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height-300,0.1f)) * Matrix4::Scale(Vector3((float)width/12,(float)height/20,1));
			UpdateShaderMatrices();
			newGame->Draw();
			modelMatrix.ToIdentity();
			
		
			modelMatrix.ToIdentity();
			modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height- 400,0.1f)) * Matrix4::Scale(Vector3((float)width/12,(float)height/20,1));
			UpdateShaderMatrices();
			credits->Draw();
			modelMatrix.ToIdentity();
			modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height-500,0.1f)) * Matrix4::Scale(Vector3((float)width/12,(float)height/20,1));
			UpdateShaderMatrices();
			exitGame->Draw();

		}
		
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);

		Vector2 mpos = Window::GetMouse()->GetAbsolutePosition();

		modelMatrix = Matrix4::Translation(Vector3(mpos.x, height - mpos.y, 0.95f)) * 
					  Matrix4::Scale(Vector3(25.0f, 25.0f, 1.0f));
		checkBounds(mpos);
		
		UpdateShaderMatrices();		
		cursorQuad->Draw();
		if (!creditsFlag)
		{
			modelMatrix = Matrix4::Translation(Vector3(quadPos.x-200,(float)height - quadPos.y,0.3f)) * Matrix4::Scale(Vector3((float)width/55,(float)height/30,1));
			UpdateShaderMatrices();		
			//circle->Draw();
			modelMatrix = Matrix4::Translation(Vector3(quadPos.x+200,(float)height - quadPos.y,0.3f)) * Matrix4::Scale(Vector3((float)width/55,(float)height/30,1));
			UpdateShaderMatrices();		
			//circle->Draw();
		}

		
		glEnable(GL_BLEND);

	}
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float) width/ (float) height, 45.0f);
	UpdateShaderMatrices();
	glUseProgram(0);
	SwapBuffers();
	
}

void MenuRenderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective)	{
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text,*basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if(perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);
	}
	else{	
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x,height-position.y, position.z)) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

void MenuRenderer::checkBounds(Vector2 mousePos)
{
	//if (((kingPooka->GetWorldTransform().GetPositionVector() - flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector()).Length() < 50

	Vector2 newGamePos = Vector2((float)width/2,(float)300);
	Vector2 exitGamePos = Vector2((float)width/2,(float)500);
	//Vector2 continueGamePos = Vector2((float)width/2,(float)300);
	
	Vector2 creditsPos = Vector2((float)width/2,(float)400);
	
	
	if (( mousePos- newGamePos ).Length() < 50)// ||(  newGamePos - mousePos ).Length() < 50)
	{
		if (Window::GetMouse()->ButtonTriggered(MOUSE_LEFT))
		{
			start = true;
		}		
		quadPos =  newGamePos;		
	}

	
	if (( mousePos- exitGamePos ).Length() < 50)
	{
		if (Window::GetMouse()->ButtonTriggered(MOUSE_LEFT))
		exitFlag = true;

		quadPos = exitGamePos;
		
	}
	

	if (( mousePos- creditsPos ).Length() < 50)
	{
		if (Window::GetMouse()->ButtonTriggered(MOUSE_LEFT))
		creditsFlag = true;

		quadPos =  creditsPos;		
	}	
}