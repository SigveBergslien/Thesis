#include "Renderer.h"

// Constructor
Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	yaw = 0;
	pitch =0;
	//replay
	//replayFile.open ("test.txt");
	msPast = 0;
	resetReplay = false;

	replaying =false;
	fpReplay= false;
	readPos=17;
	tacReadPos = 17;
	kbReadPos = 17;
	msReadPos = 14;
	viewReadPos= 13;


	playingSound = true;		//Turn music off or on
	flowersEaten = 0;			//Used for total score
	preditorKilled = 0;			//Used for total score
	pauseCounter = 0;			//hack to get sound to pause :)
	flagRot = 1.0f;
	flagRot2 = 1.0f;
	
	unsigned int seed;
	if(replaying)
	{
		string line;
		ifstream msreplayFile ("seeds.txt");
		getline (msreplayFile,line);
		string tmp = "";
		for(int i =0; i<line.size();i++)
		{
			if(line[i] != '_')
			{
				tmp += line[i];
			}
			else
			{
				seed = atoi(tmp.c_str());
			}
		}
	}
	else
	{
		seed = (unsigned int)time(NULL);

		//for replay
		ofstream msreplayFile;
		msreplayFile.open ("seeds.txt");
		if(msreplayFile.is_open())
			msreplayFile<<seed<<"_\n";
		msreplayFile.close();

		ofstream replayFile;
		replayFile.open ("replayFile.txt");
		if(replayFile.is_open())
			replayFile<<"Renderer Replay"<<"\n";
		replayFile.close();

		ofstream tacReplayFile;
		tacReplayFile.open ("tacReplay.txt");
		if(tacReplayFile.is_open())
			tacReplayFile<<"Tactical Replay"<<"\n";
		tacReplayFile.close();

		ofstream msFPreplayFile;
		msFPreplayFile.open ("msReplay.txt");
		if(msFPreplayFile.is_open())
			msFPreplayFile<<"Mouse Replay\n";
		msFPreplayFile.close();

		ofstream viewReplayFile;
		viewReplayFile.open ("viewReplay.txt");
		if(viewReplayFile.is_open())
			viewReplayFile<<"View Replay\n";
		viewReplayFile.close();
	}

	srand(seed);

	pause = false;
	endSceneOver = false;
	frames = false;

	// Quad for full-screen rendering
	quad = Mesh::GenerateQuad();
	crosshair = Mesh::GenerateQuad();
	crosshair->SetTexture(SOIL_load_OGL_texture("../Textures/crosshair.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//quad for tutorials
	tutorialQuad = Mesh::GenerateQuad();
	tutorialQuad->SetTexture(SOIL_load_OGL_texture("../Textures/openingT.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	tutFlowerQuad = Mesh::GenerateQuad();
	tutFlowerQuad->SetTexture(SOIL_load_OGL_texture("../Textures/flowerT.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	tutPredatorQuad = Mesh::GenerateQuad();
	tutPredatorQuad->SetTexture(SOIL_load_OGL_texture("../Textures/predatorT.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	tutCrystalQuad = Mesh::GenerateQuad();
	tutCrystalQuad->SetTexture(SOIL_load_OGL_texture("../Textures/crystalT.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	tutPookaQuad = Mesh::GenerateQuad();
	tutPookaQuad->SetTexture(SOIL_load_OGL_texture("../Textures/pookaT.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	endGameQuad = Mesh::GenerateQuad();
	endGameQuad->SetTexture(SOIL_load_OGL_texture("../Textures/gameover.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	pauseQuad = Mesh::GenerateQuad();
	pauseQuad->SetTexture(SOIL_load_OGL_texture("../Textures/menuScreen.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	pauseExit = Mesh::GenerateQuad();
	pauseExit->SetTexture(SOIL_load_OGL_texture("../Textures/exitGame.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	pauseContinue= Mesh::GenerateQuad();
	pauseContinue->SetTexture(SOIL_load_OGL_texture("../Textures/continueGame.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	tutMeteorQuad = Mesh::GenerateQuad();
	tutMeteorQuad->SetTexture(SOIL_load_OGL_texture("../Textures/meteorT.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	//Data driven part
	if(myParser.Parser("../Data/lvlonegamedata.xml", (void*)&gameData, GAME))
	{
		//cout << "Game Data imported\n";
	}
	else
	{
		//cout << "Error! Reading XML File: ../Data/lvlonegamedata.xml \n";
	}

	Vector4 tmpcolours[4];
	tmpcolours[0] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	tmpcolours[1] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	tmpcolours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	tmpcolours[3] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	bar1 = Mesh::GenerateQuadWithColour(tmpcolours);

	tmpcolours[0] = Vector4(0.0f, 0.0f, 0.2f, 1.0f);
	tmpcolours[1] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	tmpcolours[2] = Vector4(0.0f, 0.0f, 0.2f, 1.0f);
	tmpcolours[3] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	bar2 = Mesh::GenerateQuadWithColour(tmpcolours);


	// Perspective projection
	setPerspectiveProjection();


	// Camera
	//camera = new Camera(40.0f, 130.0f, Vector3((RAW_WIDTH * HEIGHTMAP_X / 3.0f), 1500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 3.0f)), (float)width, (float)height);
	camera = new Camera(50.0f, 140.0f, Vector3(300.0f, 300.0f, 300.0f), (float)width, (float)height,replaying);

	// Minimap camera
	minimapCam = new minimapCamera(90, 180, Vector3((RAW_WIDTH * HEIGHTMAP_X), 20000.0f, 0.0f));


	// Cube map
	{
		cubeMap = SOIL_load_OGL_cubemap("../Textures/snow_west.jpg", "../Textures/snow_east.jpg", 
										"../Textures/snow_up.jpg", "../Textures/snow_down.jpg", 
										"../Textures/snow_south.jpg", "../Textures/snow_north.jpg", 
										SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
		if (!cubeMap)
		{
			return;
		}
	}


	// Height map
	{
		// Create height map from .raw file
		//heightMap = new HeightMap("../Textures/terrain.raw");
		//heightMap = new HeightMap("../Framework/config.cfg", 1);
		heightMap = new HeightMap("../Framework/defaultTerrain_1.txt", 1, 1);
		//heightMap->generateFurTex(50000,0.03f,1.0f);
		
		// Textures
		heightMap->SetTexture(SOIL_load_OGL_texture("../Textures/grasstex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		heightMap->SetTexture2(SOIL_load_OGL_texture("../Textures/rock.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		heightMap->SetTexture3(SOIL_load_OGL_texture("../Textures/snow.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

		if (!heightMap->GetTexture() || !heightMap->GetTexture2() || !heightMap->GetTexture3())
		{
			return;
		}

		// Bump maps
		heightMap->SetBumpMap(SOIL_load_OGL_texture("../Textures/grassNormal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		heightMap->SetBumpMap2(SOIL_load_OGL_texture("../Textures/rockNormal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		heightMap->SetBumpMap3(SOIL_load_OGL_texture("../Textures/snowNormal.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		
		if (!heightMap->GetBumpMap() || !heightMap->GetBumpMap2() || !heightMap->GetBumpMap3())
		{
			return;
		}

		camera->setHeightMap(heightMap);

		// Height map texture repeating
		SetTextureRepeating(heightMap->GetTexture(), true);
		SetTextureRepeating(heightMap->GetTexture2(), true);
		SetTextureRepeating(heightMap->GetTexture3(), true);
		SetTextureRepeating(heightMap->GetBumpMap(), true);
		SetTextureRepeating(heightMap->GetBumpMap2(), true);
		SetTextureRepeating(heightMap->GetBumpMap3(), true);

		// Get the height map max height
		heightMapMaxHeight = heightMap->getMaxHeight();
	}

	srand(seed);

	specVal = 1.0f;
	fogDis = 600.0;

	// Shaders
	{
		// Cube mapping
		skyboxShader = new Shader("../Shaders/skyboxVertex.glsl", "../Shaders/skyboxFragment.glsl");
		if (!skyboxShader->LinkProgram())
		{
			return;
		}
		//

		// Shadow mapping
		shadowShader = new Shader("../Shaders/shadowVertex.glsl", "../Shaders/shadowFragment.glsl");
		if (!shadowShader->LinkProgram())
		{
			return;
		}
		//

		// Deferred rendering
		gBufferShader = new Shader("../Shaders/gBufferVertex.glsl", "../Shaders/gBufferFragment.glsl");
		if (!gBufferShader->LinkProgram())
		{
			return;
		}

		directionalLightShader = new Shader("../Shaders/directionalLightVertex.glsl", "../Shaders/directionalLightFragment.glsl");
		if (!directionalLightShader->LinkProgram())
		{
			return;
		}

		pointLightShader = new Shader("../Shaders/pointLightVertex.glsl", "../Shaders/pointLightFragment.glsl");
		if (!pointLightShader->LinkProgram())
		{
			return;
		}

		combineShader = new Shader("../Shaders/combineVertex.glsl", "../Shaders/combineFragment.glsl");
		if (!combineShader->LinkProgram())
		{
			return;
		}
		//

		//// Fur shader
		//furShader = new Shader("../Shaders/furVertex.glsl", "../Shaders/furFragment.glsl");
		//if (!furShader->LinkProgram())
		//{
		//	return;
		//}
		//

		// Minimap shader
		minimapShader = new Shader("../Shaders/minimapVert.glsl", "../Shaders/minimapFrag.glsl");
		if (!minimapShader->LinkProgram())
		{
			return;
		}
		//

		// HUD shader
		hudShader = new Shader("../Shaders/hudVertex.glsl", "../Shaders/hudFragment.glsl");
		if (!hudShader->LinkProgram())
		{
			return;
		}
		//

		spellShader = new Shader("../Shaders/ParticleVertex.glsl", "../Shaders/ParticleFragment.glsl", "../Shaders/ParticleGeometry.glsl");
	
		if(!spellShader->LinkProgram())
		{
			return;
		}

		// Cursor shader
		cursorShader = new Shader("../Shaders/cursorVertex.glsl", "../Shaders/cursorFragment.glsl");
		if (!cursorShader->LinkProgram())
		{
			return;
		}

		debugShader = new Shader("../Shaders/basicVertexWorldTransform.glsl", "../Shaders/colourFragment.glsl");
		if(!debugShader->LinkProgram())
		{
			return;
		}
	}

	
	// Deferred rendering buffers & textures
	{
		glGenFramebuffers(1, &G_FBO);
		glGenFramebuffers(1, &L_FBO);

		GLenum buffers[4];
		buffers [0] = GL_COLOR_ATTACHMENT0;
		buffers [1] = GL_COLOR_ATTACHMENT1;
		buffers [2] = GL_COLOR_ATTACHMENT2;
		buffers [3] = GL_COLOR_ATTACHMENT3;

		// Generate our scene depth texture
		generateScreenTexture(G_DepthTex, true);
		generateScreenTexture(G_ColourTex);
		generateScreenTexture(G_NormalTex);
		generateScreenTexture(G_MaterialTex);
		generateScreenTexture(L_EmissiveTex);
		generateScreenTexture(L_SpecularTex);
		generateScreenTexture(fogofwarTex);

		// And now attach them to our FBOs
		glBindFramebuffer(GL_FRAMEBUFFER, G_FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, G_ColourTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, G_NormalTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, G_MaterialTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, fogofwarTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,	 GL_TEXTURE_2D, G_DepthTex, 0);
		glDrawBuffers(4, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, L_FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, L_EmissiveTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, L_SpecularTex, 0);
		glDrawBuffers(2, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	// Shadow mapping buffers & textures
	{
		glGenTextures(1, &shadowTex);
		glBindTexture(GL_TEXTURE_2D, shadowTex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		
		glBindTexture(GL_TEXTURE_2D, 0);


		glGenFramebuffers(1, &shadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
		glDrawBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !shadowTex)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	// Minimap buffer & shader
	{
		glGenTextures(1, &minimapTex);
		glBindTexture(GL_TEXTURE_2D, minimapTex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);


		glGenFramebuffers(1, &minimapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, minimapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, minimapTex, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !minimapTex)
		{
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	// HUD
	{
		minimapDimension = (float)width / 15.0f;

		OBJMesh *mAvatarMesh = new OBJMesh();
		mAvatarMesh->LoadOBJMesh("../Meshes/ico.obj");
		avatarMesh = mAvatarMesh;

		textureCreature = SOIL_load_OGL_texture("../Textures/creature.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
		textureCrystal = SOIL_load_OGL_texture("../Textures/diamond.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
		basicFont = new Font(SOIL_load_OGL_texture("../Textures/characterSet.tga",SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT),16,16);

		textMeshScore = new TextMesh("000000000", *basicFont);
		textMeshLevel = new TextMesh("0000000", *basicFont);
		textMeshCreatures = new TextMesh("00000", *basicFont);
		textMeshCrystals = new TextMesh("00000", *basicFont);
		textMeshFrames = new TextMesh("0000000", *basicFont);
	}


	// Cursor meshes & textures
	{
		triangle = Mesh::GenerateTriangle();

		circle = new OBJMesh("../Meshes/circle.obj");
		circle->SetTexture(SOIL_load_OGL_texture("../Textures/bgreen.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		if (!circle->GetTexture())
		{
			return;
		}
		SetTextureRepeating(circle->GetTexture(), true);
		
		arrow = new OBJMesh("../Meshes/arrow.obj");
		arrow->SetTexture(SOIL_load_OGL_texture("../Textures/bgreen.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		if (!arrow->GetTexture())
		{
			return;
		}
		SetTextureRepeating(arrow->GetTexture(), true);

		// Mouse buttons flags
		leftClick = false;
		rightClick = false;

		// Cursor texture
		cursorTex = SOIL_load_OGL_texture("../Textures/select.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	}

	//physics
	engine = new PhysicsThread;
	engine->initialize(parent);
	engine->setHeightMap(heightMap);
	engine->setFrustum(frameFrustum);


	//astar
	AstarThread = new AStarThread();
	AstarThread->initialise(heightMap);

	destination = Vector3(500,0,500);
	msecCounter = 0;

	
	//to set initial pooka jump positions
		for (int i = 0; i < gameData.getNumberExtraPookas()+gameData.getNumberStartingPookas(); i++)
		{
			pookaJumpArray[i] = (float)(rand() % 360);
			
		}
		bumpMapping = false;

	//root = new SceneNode();
	//function to set up meshes and apply to scenenodes
	makeMeshes();
	//Setup the sound of the game
	if(playingSound)
		SetupSound();
	populateScenenodes();
	
	meteorTime = (rand() % 100);
	meteorTime += 60;
	

	Vector3 tmp = Vector3(0,30,0);
	Vector3 tmp1  = pedestal[0]->GetTransform().GetPositionVector()+tmp;
	Vector3 tmp2 = pedestal[1]->GetTransform().GetPositionVector()+tmp;
	Vector3 tmp3 = pedestal[2]->GetTransform().GetPositionVector()+tmp;
	Vector3 tmp4 = pedestal[3]->GetTransform().GetPositionVector()+tmp;

	cape = new Cloth(25,25,5,tmp3,tmp2,tmp4,tmp1);
		cape->SetTexture(SOIL_load_OGL_texture("../Textures/space5.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		SetTextureRepeating(cape->GetTexture(), true);
		engine->SetCape(cape);


	// Light(s)
	{
		// Main light
		light = new Light(Vector3(2000.0f, 4000.0f, 0.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 20000.0f);

		// Point lights
		lightsNumber = gameData.getNumberMushrooms() + 5;

		//pointLights = new Light[LIGHTNUM];
		pointLights = new Light[lightsNumber];
		
	
		// main light
		//pointLights[0].SetPosition(Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)));
		pointLights[0].SetPosition(Vector3(2000.0f, 3000.0f, 0.0f));
		pointLights[0].SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		pointLights[0].SetRadius(20000.0f);


		//for (int i = 0; i < 50 && lightArray[i] != Vector3(0,0,0); i++)
		//{		
		//}
/*		
		for (unsigned int i = 0; i < gameData.getNumberMushrooms(); ++i)
		{
			float r = 0.5f + ( float )( rand ()%129) / 128.0f;
			float g = 0.5f + ( float )( rand ()%129) / 128.0f;
			float b = 0.5f + ( float )( rand ()%129) / 128.0f;

			Vector3 pos = mushroomNode[i]->GetTransform().GetPositionVector();
			pos.y += 50;

			pointLights[i+1].SetPosition(pos);
			pointLights[i+1].SetColour(Vector4(r, g, b, 1.0f));
			pointLights[i+1].SetRadius(100.0f);
		}
*/

		// Light mesh volume
		lightSphere = new OBJMesh();
		if (!lightSphere->LoadOBJMesh("../Meshes/ico.obj"))
		{
			return;
		}
	}

	
	engine->addToFlock();


	int nodeNumber = 0;
	leftSinNumber = 180;
	rightSinNumber = 0;
	leftHandYpos = 0;
	rightHandYpos = 0;

	debugEnabled = false;
	firstPerson = false;
	firstPass = false;
	meteorCount = 0;			//To make sure the sound for meteor will only play ones
	crystalBell = true;		//To make sure the sound for tutorial will only play ones
	CameraUpdateF = &Camera::UpdateCamera;

	spell = new Spells();
	weatherEffect = new WeatherEmitter();
	gameData.setMana(500.0f);
	endScene = false;
	endScene1 = false;
	endScene2 = false;
	openingTutFlag = true;
	flowerTutFlag = false;
	predatorTutFlag = false;
	pookaTutFlag = false;
	endGameFlag = false;
	crystalTutFlag = false;
	delHNode = true;
	delSNode = true;
	delLNode = true;
	crystalRotation = 1.0f;

	// fog of war
	alliesRadius = 500.0f;
	haveFogofwar = true;

	// OpenGL & others
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		init = true;
	}
	for (unsigned int i = 0; i < lightsNumber; ++i)
		{
			float r = 0.5f + ( float )( rand ()%129) / 128.0f;
			float g = 0.5f + ( float )( rand ()%129) / 128.0f;
			float b = 0.5f + ( float )( rand ()%129) / 128.0f;

			Vector3 pos = lightArray[i];
			//pos.y = 0;

			pointLights[i].SetPosition(pos);
			pointLights[i].SetColour(Vector4(1, 0, 0, 1.0f));
			pointLights[i].SetRadius(50.0f);
		}
	tutorialFlag = false;
	gameOverFlag = false;
	fullGameOver = false;
	meteorTutFlag = false;
	spell->SetRigidBody(engine);
	AstarThread->start();
	engine->start();
	delayTime = time(0);
	currTime = difftime(time(0),delayTime);
	snowCheck = 0;
	snowFactor = 0.0f;
	fogOverride = 0.95f;
	fogColor = Vector3(0.5, 0.5, 0.5);

} //END OF RENDER METHOD

// Destructor
Renderer::~Renderer(void)
{
	engine->stop();
	engine->join();
	// Shaders
	delete skyboxShader;
	
	delete shadowShader;
	
	delete gBufferShader;
	delete directionalLightShader;
	delete pointLightShader;
	delete combineShader;
	
	delete minimapShader;
	delete hudShader;

	//delete furShader;
	delete cursorShader;
	
	currentShader = NULL;

	// Meshes
	
	//quad for tutorials
	delete quad;
	delete pauseQuad;
	delete pauseContinue;
	delete tutorialQuad;
	delete pauseExit;
	delete tutFlowerQuad;
	delete tutCrystalQuad;
	delete tutPookaQuad;
	delete endGameQuad;
	delete tutPredatorQuad;
	delete lightSphere;
	delete heightMap;
	delete avatarMesh;
	delete triangle;
	delete circle;
	delete arrow;
	delete leftHand;
	delete rightHand;
	delete cape;
	delete spell;
	delete tree;
	delete kingPookaMesh;
	delete pooka;
	delete flowerHead;
	delete flowerLeaves;
	delete flowerStem;

	//states
	delete kingState;
	for (int i = 0; i< gameData.getNumberPredators(); i++)	
		delete predatorState[i];

	// Shadow mapping buffer & texture
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);


	// Deferred rendering buffers & textures
	glDeleteTextures(1, &G_ColourTex);
	glDeleteTextures(1, &G_NormalTex);
	glDeleteTextures(1, &G_MaterialTex);
	glDeleteTextures(1, &G_DepthTex);
	glDeleteTextures(1, &fogofwarTex);
	glDeleteTextures(1, &L_EmissiveTex);
	glDeleteTextures(1, &L_SpecularTex);

	glDeleteFramebuffers(1, &G_FBO);
	glDeleteFramebuffers(1, &L_FBO);


	// Minimap buffers & textures
	glDeleteTextures(1, &minimapTex);
	glDeleteFramebuffers(1, &minimapFBO);


	// Camera & light
	delete camera;
	delete minimapCam;
	delete [] pointLights;


	// SceneNode Root
	delete root;

	AstarThread->stop();
	AstarThread->join();
	delete AstarThread;
	
	engine->stop();
	engine->join();
	delete engine;

	if(playingSound)
	{
		Sound::DeleteSounds();
		SoundSystem::Destroy(); 
	}
	//replayFile.close();
}

// updates the scene												
void Renderer::UpdateScene(float msec)
{
	if(msec<1000)
		msPast +=msec;

	if(resetReplay)
	{
		engine->stop();
		engine->join();
		int seed;
		string line;
		ifstream msreplayFile ("seeds.txt");
		getline (msreplayFile,line);
		string tmp = "";
		for(int i =0; i<line.size();i++)
		{
			if(line[i] != '_')
			{
				tmp += line[i];
			}
			else
			{
				seed = atoi(tmp.c_str());
			}
		}
		if(firstPerson)
			camera->toggleFirstPerson();
		srand(seed);
		msPast =0;
		gameOverFlag = true;
		Sleep(100);
		delete engine;
		engine = new PhysicsThread;
		engine->initialize(*win);
		engine->setHeightMap(heightMap);
		engine->SetCape(cape);
		engine->setFrustum(frameFrustum);
		flowersEaten = 0;			//Used for total score
		preditorKilled = 0;			//Used for total score
		cleanNodes();
		levelReset();
		heightMap->resetObjList();
		populateScenenodes();		

		spell->SetRigidBody(engine);
		engine->addToFlock();
		Sleep(15000);
		//gameData.incrementLevel();
		meteorTime = (rand() % 100);
		meteorTime += 60;
		
		gameOverFlag = false;
		delayTime = time(0);
		meteorCount = 0;  //To make sure the sound for meteor will only play ones
		for (unsigned int i = 0; i < lightsNumber; ++i)
		{
			float r = 0.5f + ( float )( rand ()%129) / 128.0f;
			float g = 0.5f + ( float )( rand ()%129) / 128.0f;
			float b = 0.5f + ( float )( rand ()%129) / 128.0f;

			Vector3 pos = lightArray[i];
			//pos.y = 0;

			pointLights[i].SetPosition(pos);
			pointLights[i].SetColour(Vector4(1, 0, 0, 1.0f));
			pointLights[i].SetRadius(50.0f);
		}
		resetReplay = false;
		engine->start();
	}
	if(endSceneOver)
	{
		msPast =0;
		gameData.setLastScore(totalScore + gameData.getLastScore());
		endScene = false;
		gameOverFlag = true;
		engine->stop();
		engine->join();
		delete engine;
		engine = new PhysicsThread;
		engine->initialize(*win);
		engine->setHeightMap(heightMap);
		engine->SetCape(cape);
		engine->setFrustum(frameFrustum);
		endSceneOver = false;
		flowersEaten = 0;			//Used for total score
		preditorKilled = 0;			//Used for total score
		cleanNodes();
		levelReset();
		populateScenenodes();		

		spell->SetRigidBody(engine);
		engine->addToFlock();
		Sleep(2000);
		gameData.incrementLevel();
		engine->start();
		meteorTime = (rand() % 100);
		meteorTime += 60;
		
		gameOverFlag = false;
		delayTime = time(0);
		meteorCount = 0;  //To make sure the sound for meteor will only play ones
		for (unsigned int i = 0; i < lightsNumber; ++i)
		{
			float r = 0.5f + ( float )( rand ()%129) / 128.0f;
			float g = 0.5f + ( float )( rand ()%129) / 128.0f;
			float b = 0.5f + ( float )( rand ()%129) / 128.0f;

			Vector3 pos = lightArray[i];
			//pos.y = 0;

			pointLights[i].SetPosition(pos);
			pointLights[i].SetColour(Vector4(1, 0, 0, 1.0f));
			pointLights[i].SetRadius(50.0f);
		}
		
		//Data driven write to file
		/*string name = "../Data/lvl";
		stringstream s;
		s << name << gameData.getLevel() << "_";
		if(myParser.WriteToFile(s.str(), (void*)&gameData, GAME))
		{
			cout << "Game Data writen\n";
		}
		else
		{
			cout << "Error! Writing XML File: ../Data/lvlone.xml \n";
		}*/

	}

	if(pause)
	{
		
		viewMatrix = camera->BuildViewMatrix();
		Sleep(1);
	}
	else{
		
		gameData.setNumberOfPookas(engine->getNumberInGroup());
		//weatherEffect->Update(msec, camera->GetPosition());
		gameData.increaseMana();
		//spell
		spell->Update(msec);
		
		if(spell->isActive() && spell->checkCollision())
		{
			if(heightMap->surrondingVerts(spell->getPosition()))
			{
				spell->spellHit();
			}
			else if(spell->isActive() && (spell->gameObj.getState() == DEAD))
			{
				spell->spellHit();
			}
		}

		this->msec = msec;
		toggleDebug();
		toggleFrames();
		//engine->toggleClothUpdate();
		//cape->update(msec);
		//engine->toggleClothUpdate();
		checkKeyPress(msec, msPast);
		toggleFirstPerson();
		//camera->UpdateCamera(msec, heightMap->getGroundPos(camera->GetPosition()));
		if(!endScene)(camera->*CameraUpdateF)(msPast,msec, heightMap->getGroundPos(camera->GetPosition()));

		if(firstPerson)
		{
			mouseFPCheck(msec);
			//Vector3 tmp = camera->GetPosition();
			//tmp.y = 0;
			//Matrix4 tmp1 = camera->BuildCameraMatrix();
			//tmp1.values[13] = 0.0f;
			////kingPooka->SetTransform(tmp1);
			////kingPooka->SetTransform(Matrix4::Translation(tmp));
			////engine->setPositionKing(tmp);
			//engine->setPositionKing(tmp.GetPositionVector());
		}
		else
		{
			if(fpReplay)
			{
				Matrix4 tmp = kingPooka->GetTransform();
				Vector3 tmppos = Vector3(0,0,0);
				if(engine->getBody(kingPooka->GetRigidBody()))
					tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();
				tmppos.y = kingPooka->GetBoundingRadius();
				camera->SetPosition(tmppos);
				camera->SetYaw(tmp.RotationMatrixToYaw());
				camera->SetPitch(pitch);
				Window::GetMouse()->SetAbsolutePosition(width/2, height/2);
			}
			kingPooka->setFPS(false);
		}

		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(ZNEAR, 1700, (float)width / (float)height, FOV);
		frameFrustum.FromMatrix(projMatrix*viewMatrix);
		setPerspectiveProjection();

		for(int i = 0; i < 4; i++)
		{
			if(crystal[i] != NULL)
			{
				if(!endScene) crystal[i]->SetTransform(Matrix4::Translation(crystal[i]->GetTransform().GetPositionVector()) * Matrix4::Rotation(45.0f, Vector3(0.0f, 1.0f, 0.0f)) * Matrix4::Rotation(90.0f, Vector3(0.0f, 0.0f, 1.0f)) * Matrix4::Rotation(crystalRotation, Vector3(0.0f, 1.0f, 0.0f)));
				else  crystal[i]->SetTransform(Matrix4::Translation(crystal[i]->GetTransform().GetPositionVector()) * Matrix4::Rotation(crystalRotation, Vector3(0.0f, 1.0f, 0.0f)));
			}
		}

		crystalRotation += 1.0f;
		root->Update(msec);	
		
		if(!AstarThread->MakingPath()&&kingPooka->getNeedPath())
		{
			kingPooka->setPath(AstarThread->GetPath(), AstarThread->getPathLength());
		}
		//for the jumping function
		for (int i = 0; i < gameData.getNumberStartingPookas() + gameData.getNumberExtraPookas(); i++)
		{
			if (pookaNode[i]->objData.getState() != DEAD)
				jumpFunc(i);
		}

		if(engine->getBody(0))
		{

			if (engine->getBody(0)->GetVelocity().SqrLength() > 100)
			{
				handMove(kingLeft);
				handMove(kingRight);
			}
		}
		
		//Remove sound nodes
		if(playingSound)
			RemoveDeadSoundNodes();
	}
	
	endSceneCheck();

	if(endScene1)
	{
		
		for(int i = 0; i < 4; i++)
		{
			endPortal[i]->UpdateStream(msec, true);
		}
		if(endScene2) endPortal[4]->UpdateStream(msec, true);
	}

	if (difftime(time(0),delayTime) == meteorTime)
	{		
		meteorNode->objData.setState(ALIVE);
		meteorCount++;
		meteorTutFlag = true;
	}
	

	if(playingSound && meteorCount == 1)
	{
		SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/bamg.wav"),SOUNDPRIORITY_HIGH);
		meteorCount++;
	}
		
	if(playingSound)
	{
		SoundSystem::GetSoundSystem()->Update(msec);
	}

	// Snow
	if (snowCheck == 1)
	{
		snowFactor += msec / 20000.0f;
		snowFactor = min(snowFactor, 1.0f);
	}
	else
	{
		snowFactor -= msec / 500000.0f;
		snowFactor = max(snowFactor, 0.0f);
	}

	if(pauseCounter >= 1)
		pauseCounter++;

	if(pauseCounter > 3)
		realTogglePause();

}

// renders the scene
void Renderer::RenderScene()
{
	wglMakeCurrent(deviceContext, renderContext);
	//Build nodes
	BuildNodeLists(root);
	SortNodeLists();
	
	DetectCrystalPickup();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	firstPass = true;
	drawShadowScene();
	firstPass = false;
	glDisable(GL_CULL_FACE);

	if (debugEnabled)
	{
		drawDebug();
		drawSpell();
		//drawWeather();
	}
	else
	{
		fillBuffers();
		lightingPass();
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		combineBuffers();
		glDisable(GL_CULL_FACE);
		GL_BREAKPOINT
		if (gameData.getLevel() == 1)
			//tutorialFunc();
		drawHUD();	
	}
	if (difftime(time(0), delayTime) > 5)
		if (gameData.getNumberOfPookas()-1 <= 0)
			{			
				endGame();	
			}

	if (kingPooka->objData.getState() == DEAD)	
		endGameFlag = true;
	

	SwapBuffers();
	glUseProgram(0);

	//Clean up nodes
	ClearNodeLists();

}

void Renderer::levelReset()
{
	setPerspectiveProjection();
	camera->SetPitch(50.0f);
	camera->SetYaw(140.0f);
	camera->SetPosition(Vector3(300.0f, 300.0f, 300.0f));
	if(resetReplay)
	{
		camera->toggleReplay();
	}
	else
	{
		heightMap->generateMap();
	}
	heightMapMaxHeight = heightMap->getMaxHeight();

	debugEnabled = false;
	firstPerson = false;
	firstPass = false;

	CameraUpdateF = &Camera::UpdateCamera;	
	gameData.setCrystalsCollected(0);
	gameData.setMana(500.0f);
	endScene = false;
	endScene1 = false;
	endScene2 = false;

	for (int i = 0; i < gameData.getNumberStartingPookas() + gameData.getNumberExtraPookas(); i++)
	{
		pookaJumpArray[i] = (float)(rand() % 360);
	}
	bumpMapping = false;
}

void Renderer::cleanNodes()
{
	for (int i = 0; i< gameData.getNumberPredators(); i++)	
	{
		delete predatorState[i];
	}

	delete kingState;
	delete root;

	for (int i = 0; i < 4; i++)
	{
		delete endPortal[i];
	}
}

// Toggles the view mode between First person view and Strategic view
void Renderer::toggleViewMode()
{
	camera->toggleViewMode();
}

// Toggles free view on camera
void Renderer::toggleFreeView()
{
	camera->toggleFreeView();
}

// Enables/disables cel shading
void Renderer::toggleBumpmapping()
{
	bumpMapping = !bumpMapping;
}

// Sets the perspective projection
void Renderer::setPerspectiveProjection()
{
	projMatrix = Matrix4::Perspective(ZNEAR, ZFAR, (float)width / (float)height, FOV);
}

void Renderer::generateScreenTexture(GLuint &into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8,	width, height, 0, 
								   depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::drawSkybox()
{
	glDepthMask(GL_FALSE);

	SetCurrentShader(skyboxShader);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
	glUseProgram(0);
}

void Renderer::drawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	setPerspectiveProjection();
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3((RAW_WIDTH * HEIGHTMAP_X / 2.0f), 0.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)));
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);
	UpdateShaderMatrices();

	drawHeightmap();
	DrawNodes();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::fillBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, G_FBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	drawSkybox();

	SetCurrentShader(gBufferShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 3);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "snowFactor"), snowFactor);

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogDistance"), fogDis);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "pookaPos"), 1, (float*)&kingPooka->GetTransform().GetPositionVector());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "F_W_Radius"), alliesRadius);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowTex); 

	setPerspectiveProjection();
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	if (endScene && !endSceneOver) drawEndScene();

	drawHeightmap();
	DrawNodes();

	drawCape();

	projCheck();
	
	//if (firstPerson)
		//firstPersonFlowerCheck();
		
if (!debugEnabled)	//drawGrass();

	drawSpell();
 	//drawWeather();

	if (endScene1 && !endSceneOver) drawNexus();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::lightingPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, L_FBO);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	drawDirectionalLight();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	
	drawPointLights();

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::drawDirectionalLight()
{
	SetCurrentShader(directionalLightShader);
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 7);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, G_DepthTex);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, G_NormalTex);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	SetShaderLight(*light);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	quad->Draw();

	setPerspectiveProjection();
}

void Renderer::drawPointLights()
{
	SetCurrentShader(pointLightShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 5);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 7);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, G_DepthTex);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, G_NormalTex);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	for (int i = 0; i < lightsNumber; ++i)
	{
		Light &l = pointLights[i];
		
		float radius = l.GetRadius();

		modelMatrix = Matrix4::Translation(l.GetPosition()) *
					  Matrix4::Scale(Vector3(radius, radius, radius));

		l.SetPosition(modelMatrix.GetPositionVector());

		UpdateShaderMatrices();

		SetShaderLight(l);

		float dist = (l.GetPosition() - camera->GetPosition()).Length();
		if (dist < radius)
		{
			glCullFace(GL_FRONT);
		}
		else
		{
			glCullFace(GL_BACK);
		}

		lightSphere->Draw();
	}
}

void Renderer::combineBuffers()
{
	SetCurrentShader(combineShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 6);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 9);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 10);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "materialTex"), 8);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "fowTex"), 15);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "hfow"),  haveFogofwar);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, G_ColourTex);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, L_EmissiveTex);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, L_SpecularTex);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, G_MaterialTex);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, fogofwarTex);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
}

// draw the height map
void Renderer::drawHeightmap()
{
	modelMatrix.ToIdentity();

	Matrix4 tempMatrix = shadowMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex2"), 11);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex3"), 12);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex2"), 13);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex3"), 14);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "multiTexture"), true);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "maxHeight"), heightMapMaxHeight);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useBumpmap"), (bool)bumpMapping);

	heightMap->Draw();
}

// renders the minimap to texture
void Renderer::drawMinimap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, minimapFBO);

	glClear(GL_COLOR_BUFFER_BIT);

	SetCurrentShader(minimapShader);

	projMatrix = Matrix4::Orthographic(-1.0f, 20000.0f, (RAW_WIDTH * HEIGHTMAP_X), 0.0f, (RAW_HEIGHT * HEIGHTMAP_Z), 0.0f);
	viewMatrix = minimapCam->buildViewMatrix();
	modelMatrix.ToIdentity();

	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex2"), 11);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex3"), 12);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "multiTexture"), true);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "maxHeight"), heightMapMaxHeight);

	heightMap->Draw();


	// avatar
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "multiTexture"), false);
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "colour"), 1, (float*)&Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "avatarPos"), 1, (float*)&kingPooka->GetWorldTransform().GetPositionVector());

	if ((!firstPerson&&!replaying)||(!fpReplay&&replaying))
	{
		modelMatrix = Matrix4::Translation(kingPooka->GetWorldTransform().GetPositionVector()) * 
					  Matrix4::Scale(Vector3(70.0f, 70.0f, 70.0f));
		UpdateShaderMatrices();

		avatarMesh->Draw();
	}
	else
	{
		modelMatrix = Matrix4::Translation(camera->GetPosition()) * 
						  Matrix4::Scale(Vector3(200.0f, 200.0f, 200.0f)) *
						  Matrix4::Rotation(90.0f, Vector3(1.0f, 0.0f, 0.0f)) *
						  Matrix4::Rotation(camera->GetYaw() + 180.0f, Vector3(0.0f, 0.0f, 1.0f));
		UpdateShaderMatrices();

		triangle->Draw();
	}

	//enemies
	for (int i = 0; i < gameData.getNumberPredators(); ++i)
	{
		if(predator[i]->objData.getState() != DEAD)
		{
			Vector3 dist = predator[i]->GetTransform().GetPositionVector() - kingPooka->GetTransform().GetPositionVector();

			if(dist.Length() < alliesRadius)
			{
				modelMatrix = Matrix4::Translation(predator[i]->GetWorldTransform().GetPositionVector()) * 
							  Matrix4::Scale(Vector3(70.0f, 70.0f, 70.0f));
				UpdateShaderMatrices();

				glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "colour"), 1, (float*)&Vector4(1.0f, 0.0f, 0.0f, 1.0f));

				avatarMesh->Draw();
			}
		}
	}
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// renders the HUD on screen
void Renderer::drawHUD()
{
	drawMinimap();

	SetCurrentShader(hudShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);

	// Cursor
	{
		quad->SetTexture(cursorTex);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);

		Vector2 mpos = Window::GetMouse()->GetAbsolutePosition();
		if (pause && !tutorialFlag)
			checkBounds(mpos);

		if ((!firstPerson && !replaying) || pause  || (!fpReplay&&replaying))
		{
			modelMatrix = Matrix4::Translation(Vector3(mpos.x, height - mpos.y, 0.95f)) * 
						  Matrix4::Scale(Vector3(25.0f, 25.0f, 1.0f));
	
			UpdateShaderMatrices();
		
			quad->Draw();
		}

		else if (((firstPerson &&!replaying)||(fpReplay&&replaying))&& !pause)
		{
			modelMatrix = Matrix4::Translation(Vector3(mpos.x, height - mpos.y, 0.95f)) * 
						  Matrix4::Scale(Vector3(50.0f, 50.0f, 1.0f));
	
			UpdateShaderMatrices();

			crosshair->Draw();
		}

	
		
		
	}

	if (!pause && !endScene)
	{
		// Minimap
		 {
			
				quad->SetTexture(minimapTex);
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);

				modelMatrix = Matrix4::Translation(Vector3(minimapDimension, minimapDimension, 0.9f)) * 
							  Matrix4::Scale(Vector3(minimapDimension, minimapDimension, 1.0f)) * 

							  Matrix4::Rotation(180.0f, Vector3(0.0f, 1.0f, 0.0f)) 
							  * Matrix4::Rotation(-90,Vector3(0.0f,0.0f,1.0f));
							  Matrix4::Rotation(180.0f, Vector3(0.0f, 1.0f, 0.0f)) *
							  Matrix4::Rotation(-90.0f, Vector3(0.0f, 0.0f, 1.0f));


				UpdateShaderMatrices();

				quad->Draw();
			}
	
			// Mana bar
			{
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), false);

				modelMatrix = Matrix4::Translation(Vector3(width / 2.0f - 250.0f, 75.0f, 0.9f)) * 
							  Matrix4::Scale(Vector3(gameData.getMana(), 10.0f, 1.0f));

				UpdateShaderMatrices();

				bar1->Draw();

				modelMatrix = Matrix4::Translation(Vector3(width / 2.0f - 250.0f, 65.0f, 0.9f)) * 
							  Matrix4::Scale(Vector3(gameData.getMana(), 10.0f, 1.0f));

				UpdateShaderMatrices();

				bar2->Draw();
			}

			// Creature image
			{
				quad->SetTexture(textureCreature);

				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);

			modelMatrix = Matrix4::Translation(Vector3(width - 230.0f, 50.0f, 0.9f)) * 
						  Matrix4::Scale(Vector3(30.0f, 30.0f, 1.0f)) *
						  Matrix4::Rotation(-90.0f, Vector3(0.0f, 0.0f, 1.0f));
			viewMatrix.ToIdentity();
			projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
				modelMatrix = Matrix4::Translation(Vector3(width - 200.0f, 50.0f, 0.9f)) * 
							  Matrix4::Scale(Vector3(30.0f, 30.0f, 1.0f));
				UpdateShaderMatrices();

				quad->Draw();
			}

			//Crystal image
			{
				quad->SetTexture(textureCrystal);

				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);

			modelMatrix = Matrix4::Translation(Vector3(width - 230.0f, 120.0f, 0.9f)) * 
						  Matrix4::Scale(Vector3(30.0f, 30.0f, 1.0f)) *
						  Matrix4::Rotation(-90.0f, Vector3(0.0f, 0.0f, 1.0f));
			viewMatrix.ToIdentity();
			projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
				
			modelMatrix = Matrix4::Translation(Vector3(width - 200.0f, 120.0f, 0.9f)) * 
							  Matrix4::Scale(Vector3(30.0f, 30.0f, 1.0f));
				UpdateShaderMatrices();
				quad->Draw();

			}

			// Text
			{
				std::stringstream score(stringstream::in | stringstream::out),
								  level(stringstream::in | stringstream::out),
								  numCreatures(stringstream::in | stringstream::out),
								  numCrystals(stringstream::in | stringstream::out),
								  framerate(stringstream::in | stringstream::out);
				
				totalScore = (preditorKilled * 3) + (gameData.getNumberOfPookas()-1) + (flowersEaten * 2) + gameData.getLastScore();
				
				score << totalScore;
				string text = "Score " + score.str();
				drawText(textMeshScore, text, Vector3(0.0f, 0.0f, 0.9f), 30.0f);

				level << gameData.getLevel();
				text = "Level " + level.str();
				drawText(textMeshLevel, text, Vector3(width - 300.0f, 0.0f, 0.9f), 30.0f);

				numCreatures << gameData.getNumberOfPookas() - 1;
				text = "x" + numCreatures.str();
				drawText(textMeshCreatures, text, Vector3(width - 170.0f, height - 50.0f, 0.9f), 30.0f);

				numCrystals << gameData.getCrystalsCollected() << "/4";
				text = numCrystals.str();
				drawText(textMeshCrystals, text, Vector3(width - 170.0f, height - 120.0f, 0.9f), 30.0f);

				if(frames)
				{
					framerate << fps;
					text = "FPS: " + framerate.str();
					drawText(textMeshFrames, text, Vector3(width / 2.0f, 0.0f, 0.9f), 30.0f);
				}
			}
			
	}

	else if (pause && !endScene && !endGameFlag && !gameOverFlag && gameData.getNumberOfPookas() > 0 && !tutorialFlag)
	{
		
		pauseScreen();
	}
	
	else
	{
				SetCurrentShader(cursorShader);
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), false);
				modelMatrix = Matrix4::Translation(Vector3( (float)width/2, (float)height /2, 0.999f)) * Matrix4::Scale(Vector3(300,360,1));
				viewMatrix.ToIdentity();
				UpdateShaderMatrices();				
	}
	
		
	// Reset perspective projection
	setPerspectiveProjection();
}

// Renders text to screen
void Renderer::drawText(TextMesh *textMesh, const string &text, const Vector3 &position, const float size)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	textMesh->updateText(text);

	modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);

	UpdateShaderMatrices();
	
	textMesh->Draw();

	glDisable(GL_BLEND);
}

void Renderer::drawCape()
{
	modelMatrix.ToIdentity();
	Matrix4 tempMatrix = shadowMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);


	if(endScene2&&flagRot2<30)
	{
		flagRot += flagRot2;
		flagRot2+=0.2;
	}
	else if(endScene2)
		flagRot+=30;
	textureMatrix.ToIdentity();
	Matrix4 rotation = Matrix4::Rotation(flagRot, Vector3(0,0,1));
	Matrix4 push = Matrix4::Translation(Vector3(0.5,0.5,0.0));
	Matrix4 pop = Matrix4::Translation(Vector3(-0.5,-0.5,0.0));
	textureMatrix = push * rotation * pop;


	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&textureMatrix.values);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "multiTexture"), false);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useBumpmap"), false);

	cape->Draw();

	textureMatrix.ToIdentity();
}

// renders the grass on the terrain
void Renderer::drawGrass()
{
	SetCurrentShader(furShader);
	
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	heightMap->drawFur(currentShader->GetProgram());

	glUseProgram(0);
}

void Renderer::SortNodeLists()
{
	sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		Vector3 dir = from->GetWorldTransform().GetPositionVector()-camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}
	
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}

void Renderer::DrawNodes()
{

	for (vector<SceneNode*>::const_iterator i = nodeList.begin();
		i != nodeList.end(); ++i)
	{	
		if ((*i)->objData.getType() == HEALTHSPHERE)
		{
				if ((*i)->GetParent()->objData.getHp() >= 150)
					(*i)->SetColour(Vector4(0,1,0,1));
				if ((*i)->GetParent()->objData.getHp() < 150)
					(*i)->SetColour(Vector4(1,0.5f,0,1));
				if ((*i)->GetParent()->objData.getHp() <= 60)
					(*i)->SetColour(Vector4(1,0,0,1));
		}
		if(!(((firstPerson&&!replaying)||fpReplay&&replaying) && (*i) == kingPooka ) && !((*i)->objData.getType() == HEALTHSPHERE && (*i)->GetParent()->objData.getState() == DEAD))	DrawNode((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin();
		i != transparentNodeList.rend(); ++i)
	{
		DrawNode((*i));
	}

	//all below is to update some animations that arent in the nodelist
	int predArray[20];
	for (int i = 0; i < gameData.getNumberPredators();i++)	
		predArray[i] = i;
	
	for (vector<SceneNode*>::const_iterator i = nodeList.begin();i != nodeList.end(); ++i)
	{
		for (int j = 0; j < gameData.getNumberPredators(); j++)
		{
			if ((*i) == predator[j])
			{
				predArray[j] = -1;
				
			}
		}
	}

	if (!firstPass && !pause && !endScene)
		for (int i = 0; i < gameData.getNumberPredators(); i++)
		{
			if (predArray[i] != -1)
			{			
				{				
						predatorState[i]->UpdateState(msec);
				}
					
			}
		}
	
}

void Renderer::DrawNode(SceneNode*n)
{
	if (n->GetMesh())
	{
		
		Matrix4 tempMatrix = shadowMatrix * (n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale()));

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrix"), 1, false, *&tempMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&(n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale()).values));

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "multiTexture"), false);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useBumpmap"), false);

		if (n->GetMesh()->GetTexture())
		{
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), true);
		}
		else
		{
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), false);
		}

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&(n->GetColour()));

		if (!pause && !endScene)
		{
				if(n->objData.getType() == PREDATOR)
				{
					if (!firstPass)
					{
						// line of sight
						predatorState[((Predator*)(n))->GetNumber()]->UpdateState(msec);
						if( haveFogofwar)
						{
							Vector3 dis = n->GetWorldTransform().GetPositionVector() - kingPooka->GetTransform().GetPositionVector();		
								if (dis.Length() < alliesRadius) 
								{
									n->GetMesh()->Draw();
								}
						}
						else 
						{
							n->GetMesh()->Draw();
						}
					}
				}
				
				else if(n->objData.getType() == KINGPOOKA && !endScene)
				{
					if (firstPass)
					kingState->UpdateState(msec);
					n->GetMesh()->Draw();
				}
				
				else if(n->objData.getType() == POOKA || n->objData.getType() == HEALTHSPHERE || n->objData.getType() == CRYSTAL)
				{
					if( haveFogofwar)
						{
							Vector3 dis = n->GetWorldTransform().GetPositionVector() - kingPooka->GetTransform().GetPositionVector();		
								if (dis.Length() < alliesRadius) 
								{
									n->GetMesh()->Draw();
								}
						}
						else 
						{
							n->GetMesh()->Draw();
						}
				}
				else
					n->GetMesh()->Draw();	
		}
		else
		{			
				n->GetMesh()->Draw();
		}
	}
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::drawDebug()
{
	SetCurrentShader(debugShader);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	setPerspectiveProjection();
	UpdateShaderMatrices();

	drawHeightmap();
	drawCape();

	//Draw octree debug
	//Octree *tmp = engine->GetOctree();
	//tmp->DrawDebugCube(tmp);

	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i)
	{
		Vector3 pos;
		float rad;
		pos = (*i)->GetWorldTransform().GetPositionVector();
		rad = (*i)->GetBoundingRadius();
		pos.y = pos.y + rad;
		DebugDraw3D::DrawWireSphere3D(pos, rad, 0xffff0000);
		Vector3 to = (*i)->GetTargetPosition();
		if(to != Vector3(0.0f, 0.0f, 0.0f))
		{
			to.y = pos.y + rad;
			DebugDraw3D::DrawArrow3D(pos, to, 0xff0000ff);
		}
	}
}

void Renderer::toggleDebug()
{
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F1))
	{
		debugEnabled = !debugEnabled;

		if(debugEnabled)
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		else
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}
}

void Renderer::toggleFirstPerson()
{
	bool viewChange = false;

	if(replaying)
	{
		string line;
		int readMS;
		ifstream viewReplayFile ("viewReplay.txt");
		if (viewReplayFile.is_open())
		{
			viewReplayFile.seekg(viewReadPos);
			string tmp="";
			getline (viewReplayFile,line);
			if(line.size() >1)
				cout<<line<<endl;
			for(int i =0;i <line.length()&&line.size() >1;i++){
				if(line[i]!='_')
				{
					tmp+=line[i];
				}
				else
				{
					readMS = atoi(tmp.c_str());
					cout<<readMS<<endl;
					if(readMS<=msPast)
					{
						cout<<"test ok"<<endl;
						viewChange = !viewChange;
						viewReadPos += line.size() +2;
					}
					else
					{
						break;
					}
				}
			}
		}
	}


	if((Window::GetKeyboard()->KeyTriggered(KEYBOARD_F3) && !firstPerson && !endScene&&!replaying) || viewChange)
	{
		if(!replaying)
		{
			ofstream viewReplayFile;
			viewReplayFile.open ("viewReplay.txt",ios::app);
			if(viewReplayFile.is_open())
				viewReplayFile<<msPast<<"_F3"<<"\n";
			viewReplayFile.close();
			camera->toggleFirstPerson();
		}

		flowerCount = 0;
		rightClick = false;
		hitFlower = false;
		
		Matrix4 tmp = kingPooka->GetTransform();

		yaw = tmp.RotationMatrixToYaw();
		pitch = 0.0f;

		/*camera->SetYaw(tmp.RotationMatrixToYaw());
		camera->SetPitch(0.0f);
		camera->SetPosition(tmppos);
		CameraUpdateF = &Camera::UpdateCameraFP;	*/
		
		kingPooka->clearPath();

		firstPerson = true;
	}
	else if((Window::GetKeyboard()->KeyTriggered(KEYBOARD_F3) && firstPerson&&!replaying) || viewChange)
	{
		if(!replaying)
		{
			ofstream viewReplayFile;
			viewReplayFile.open ("viewReplay.txt",ios::app);
			if(viewReplayFile.is_open())
				viewReplayFile<<msPast<<"_F3"<<"\n";
			viewReplayFile.close();
		// put code to go back to overhead
			camera->toggleFirstPerson();
			CameraUpdateF = &Camera::UpdateCamera;
		}
		firstPerson = false;
		pitch = 0;
	}
	else if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F3) && !fpReplay && !endScene&& replaying)
	{
		camera->toggleFirstPerson();
		fpReplay=true;
	}
	else if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F3) && fpReplay&&replaying)
	{
		camera->toggleFirstPerson();
		CameraUpdateF = &Camera::UpdateCamera;
		fpReplay = false;
	}
}

void Renderer::toggleFrames()
{
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F4))
	{
		frames = !frames;
	}
}

//2D to 3D check
void Renderer::projCheck()
{
	//if there is no path being made we can turn the right click calculations off
	if (kingPooka->getInt() ==0)
	{		
		rightClick = false;	
	}

	if (Window::GetMouse()->ButtonTriggered(MOUSE_LEFT) && !pause)
	{	
		Vector3 dir = GetMouseDirectionVector3((float)width/(float)height,45.0f, *camera);	
		Ray r = Ray(camera->GetPosition(), dir);		
		r.IntersectsNodes(*root);		
		for (unsigned int i = 0; i < r.collisions.size(); ++i ) 
		{
			if(!replaying)
			{
				for (int j = 0; j < gameData.getNumberPredators(); j++)
				{
					Vector3 dis = predator[j]->GetTransform().GetPositionVector() - kingPooka->GetTransform().GetPositionVector();	
					if (dis.Length() < alliesRadius && !firstPerson)
					{
						if (predator[j]->GetWorldTransform().GetPositionVector() == r.collisions.at(i).node->GetWorldTransform().GetPositionVector())
						{					
						nodeNumber =j;
						destination = predator[j]->GetWorldTransform().GetPositionVector();
						AstarThread->setVector(kingPooka->GetTransform().GetPositionVector(), destination);
						kingPooka->setNeedPath();

						//replay
						ofstream tacReplayFile;
						tacReplayFile.open ("tacReplay.txt",ios::app);
						if(tacReplayFile.is_open())
						{
							tacReplayFile<<msPast<<"_"<<destination.x<<"_"<<destination.y<<"_"<<destination.z<<"_\n";
							tacReplayFile<<"p_"<<j<<"_\n";
						}
						tacReplayFile.close();

						leftClick = true;
						rightClick = false;
						hitPredator = true;
						hitFlower = false;
						break;
						}
					}
				}
				for (int j = 0; j < gameData.getNumberFlowers(); j++)			
				{
					if ( flowerStemNode[j]->GetWorldTransform().GetPositionVector() ==  r.collisions.at(i).node->GetWorldTransform().GetPositionVector())
					{
						nodeNumber = j; 
						destination = flowerStemNode[j]->GetWorldTransform().GetPositionVector();

						//replay
						ofstream tacReplayFile;
						tacReplayFile.open ("tacReplay.txt",ios::app);
						if(tacReplayFile.is_open())
						{
							tacReplayFile<<msPast<<"_"<<destination.x<<"_"<<destination.y<<"_"<<destination.z<<"_\n";
							tacReplayFile<<"f_"<<j<<"_\n";
						}
						tacReplayFile.close();

						AstarThread->setVector(kingPooka->GetTransform().GetPositionVector(), destination);
						kingPooka->setNeedPath();
						leftClick = true;
						rightClick = false;
						hitPredator = false;
						if (!firstPerson || ((firstPerson) && ((kingPooka->GetWorldTransform().GetPositionVector() - flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector()).Length() < 150)))
							hitFlower = true;
										
						flowerCount = 0;
						break;
					}
				}
			}		
		}
	}
	
	if(replaying)
	{
		ifstream tacReplayFile ("tacReplay.txt");
		if (tacReplayFile.is_open())
		{
			bool readNext = true;
			while( (readNext))
			{
				tacReplayFile.seekg(tacReadPos);
				string line;
				string tmp = "";
				Vector3 dest = Vector3(0,0,0);
				int counter = 0;
				getline (tacReplayFile,line);
				if(line == "")
					break;
				bool readDest = true;
				if(line[0] =='f')
				{
					readDest = false;
					tmp = line[2];
					nodeNumber = atoi(tmp.c_str());
					leftClick = true;
					hitPredator = false;
					hitFlower = true;
					flowerCount=0;
					tacReadPos += line.size() +2;
				}
				if(line[0] =='p')
				{
					readDest = false;
					tmp = line[2];
					nodeNumber = atoi(tmp.c_str());
					leftClick = true;
					hitPredator = true;
					hitFlower = false;
					flowerCount=0;
					tacReadPos += line.size() +2;
				}
				for(int i =0;i<line.size()&&readDest;i++)
				{
					if(counter ==0)
					{
						if(line[i]!='_')
						{
							tmp += line[i];
						}
						else
						{
							int readMs = atoi(tmp.c_str());
							if(readMs<= msPast)
							{
								tmp = "";
								counter++;
							}
							else 
							{
 								readNext=false;
								break;
							}
						}
					}
					else
					{
						if(line[i]!='_')
						{
							tmp += line[i];
						}
						else
						{
							float tmpCoord = atof(tmp.c_str());
							if(counter ==1)
							{
								dest += Vector3(tmpCoord,0,0);
							}
							else if(counter ==2)
							{
								dest += Vector3(0,tmpCoord,0);
							}
							else
							{
								dest += Vector3(0,0,tmpCoord);
								destination = dest;
								AstarThread->setVector(kingPooka->GetTransform().GetPositionVector(), destination);
								kingPooka->setNeedPath();
								tacReadPos += line.size() +2;
							}
							tmp ="";
							counter++;
						}
					}
				}
			}
		}
	}
	else if(Window::GetMouse()->ButtonTriggered(MOUSE_RIGHT) && !pause && !firstPerson)	
	{
		Vector3 dir = GetMouseDirectionVector3((float)width/(float)height,45.0f, *camera);		
		Ray r = Ray(camera->GetPosition(),dir);	
		destination = r.IntersectsTerrain();
		arrowPos = destination;
		destination += Vector3(0,3,0);

		//replay
		ofstream tacReplayFile;
		tacReplayFile.open ("tacReplay.txt",ios::app);
		if(tacReplayFile.is_open())
			tacReplayFile<<msPast<<"_"<<destination.x<<"_"<<destination.y<<"_"<<destination.z<<"_\n";
		tacReplayFile.close();

		arrowPos += Vector3(0,50,0);
		rightClick = true;		
		AstarThread->setVector(kingPooka->GetTransform().GetPositionVector(), destination);
		kingPooka->setNeedPath();
	}

	if (leftClick)
	{
		SetCurrentShader(cursorShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		float circleSize = 3;

		if (hitPredator && !firstPerson)
		{
			circlePos = Vector3(predator[nodeNumber]->GetWorldTransform().GetPositionVector().x, 1, predator[nodeNumber]->GetWorldTransform().GetPositionVector().z);
			circleSize = (float)predator[nodeNumber]->GetBoundingRadius()*1.5f;
			if (predator[nodeNumber]->objData.getState() == DEAD)
			{
				predator[nodeNumber]->SetBoundingRadius(0);
				leftClick = false;
				hitPredator = false;
			}
		}
		else if (hitFlower)
		{	
			
			circlePos = Vector3(flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector().x, 1, flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector().z);
			circleSize = (float)flowerStemNode[nodeNumber]->GetBoundingRadius()*1.5f;

			if (((kingPooka->GetWorldTransform().GetPositionVector() - flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector()).Length() < 50 && !firstPerson) || (firstPerson) && ((kingPooka->GetWorldTransform().GetPositionVector() - flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector()).Length() < 200))
			{	
				if (!firstPerson)
					engine->setPositionKing(kingPooka->GetWorldTransform().GetPositionVector());
				
				if (flowerCount == 1)
				{
					Vector3 pos = flowerStemNode[nodeNumber]->GetWorldTransform().GetPositionVector();
					engine->setFlockPos(pos);
					flowerClock = time(0);				
				}
				flowerCount++;
			}
				//delete the parts of the flower
				
				if (difftime( time(0),flowerClock) == 4 )		
				{
					if (delHNode)
					{						
						root->RemoveChild(flowerHeadNode[nodeNumber]);
						delHNode = false;
						//Eatting noise Sound
						if(playingSound)
							SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/apple-crunch.wav"),SOUNDPRIORITY_HIGH);
					}						
				}				

				if (difftime(time(0),flowerClock)	== 5)	
				{
					if (delLNode)
					{
						root->RemoveChild(flowerLeavesNode[nodeNumber]);
						delLNode = false;
						//Eatting noise Sound
						if(playingSound)
							SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/apple-crunch.wav"),SOUNDPRIORITY_HIGH);
					}
				}						

				if (difftime(time(0),flowerClock) == 6)
				{
					if (delSNode)
					{
						engine->resetFlockPos();
						root->RemoveChild(flowerStemNode[nodeNumber]);
						hitFlower = false;
						leftClick = false;
						delSNode = false;
						delHNode = true;
						delSNode = true;
						delLNode = true;
						gameData.setMana(gameData.getMana() + 200.0f);
						//Eatting noise Sound
						if(playingSound)
							SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/apple-crunch.wav"),SOUNDPRIORITY_HIGH);

						flowersEaten++;
						for (int i = 0; i < gameData.getNumberExtraPookas() + gameData.getNumberStartingPookas();i++)
						{
							if (pookaNode[i]->objData.getState() == INGROUP)
							{
								pookaNode[i]->SetModelScale(pookaNode[i]->GetModelScale()*1.1f);
								pookaNode[i]->SetBoundingRadius(pookaNode[i]->GetBoundingRadius() * 1.1f);
								pookaNode[i]->objData.setHp(pookaNode[i]->objData.getHp() + 50);
								if(engine->getBody(pookaNode[i]->GetRigidBody()))
								engine->getBody(pookaNode[i]->GetRigidBody())->setRadius(pookaNode[i]->GetBoundingRadius());															
							}
						
						}
					}
			}	
		}

		viewMatrix = camera->BuildViewMatrix();
		modelMatrix = Matrix4::Translation(circlePos) * Matrix4::Scale(Vector3(circleSize,1,circleSize))* Matrix4::Rotation(90,Vector3(1,0,0));
		
		UpdateShaderMatrices();
		
		circle->Draw();	
		modelMatrix.ToIdentity();
		
		setPerspectiveProjection();
		UpdateShaderMatrices();
	}

	//sets the circle and arrow in correct positions
	if (rightClick)
	{	
		 leftClick = false;
		 SetCurrentShader(cursorShader);
		 glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		 viewMatrix = camera->BuildViewMatrix();
		 modelMatrix = Matrix4::Translation(Vector3(destination)) * Matrix4::Scale(Vector3(10,5,10))* Matrix4::Rotation(90,Vector3(1,0,0));		
		 UpdateShaderMatrices();		 
		 circle->Draw();	
		 modelMatrix.ToIdentity();
		 setPerspectiveProjection();
		 UpdateShaderMatrices();
	
		 if (arrowPos.y > -100)
		 {
			 if (!pause)
				arrowPos.y -= 5;

			 modelMatrix = Matrix4::Translation(Vector3(arrowPos)) * Matrix4::Scale(Vector3(10,5,10))* Matrix4::Rotation(90,Vector3(1,0,0));
			 UpdateShaderMatrices();
			 arrow->Draw();
			 setPerspectiveProjection();
			 modelMatrix.ToIdentity();
			 UpdateShaderMatrices();
		}
		 else
		 {
			 arrowPos.y = 100;
		 }
	}
	
	//end 2D to 3d Projection
	//*************
}

void Renderer::makeMeshes()
{
	mushroom = new OBJMesh("../Meshes/mushroom.obj");
	mushroom->SetTexture(SOIL_load_OGL_texture("../Textures/gold.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!mushroom->GetTexture())
	{
		return;
	}

	tree = new OBJMesh("../Meshes/tree.obj");
	tree->SetTexture(SOIL_load_OGL_texture("../Textures/dgreen.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!tree->GetTexture())
	{
		return;
	}

	treeBark = new OBJMesh("../Meshes/treeBark.obj");
	treeBark->SetTexture(SOIL_load_OGL_texture("../Textures/bark.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!treeBark->GetTexture())
	{
		return;
	}

	//flowers 
	flowerHead = new OBJMesh("../Meshes/flowerHead.obj");
	flowerHead->SetTexture(SOIL_load_OGL_texture("../Textures/yellow.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!flowerHead->GetTexture())
	{
		return;
	}
			
	flowerStem = new OBJMesh("../Meshes/flowerStem.obj");
	flowerStem->SetTexture(SOIL_load_OGL_texture("../Textures/bgreen.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!flowerStem->GetTexture())
	{
		return;
	}
	
	flowerLeaves = new OBJMesh("../Meshes/flowerLeaves.obj");
	flowerLeaves->SetTexture(SOIL_load_OGL_texture("../Textures/bgreen.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!flowerLeaves->GetTexture())
	{
		return;
	}
			
	SetTextureRepeating(flowerLeaves->GetTexture(), true);
	SetTextureRepeating(flowerHead->GetTexture(), true);
	SetTextureRepeating(flowerStem->GetTexture(), true);

	pooka = new MD5Mesh();		
	if(!pooka->LoadMD5Mesh("../Meshes/pooka2.md5mesh")) 
	{
		return;
	}	

	if (!pooka->GetTexture())
	{
		return;
	}

	SetTextureRepeating(pooka->GetTexture(), true);
	SetTextureRepeating(pooka->GetTexture2(), true);

	kingPookaMesh = new MD5Mesh();	
	if(!kingPookaMesh->LoadMD5Mesh("../Meshes/pookaKing.md5mesh")) 
	{
		return;
	}	
		
	SetTextureRepeating(kingPookaMesh->GetTexture(), true);
	SetTextureRepeating(kingPookaMesh->GetTexture2(), true);
	SetTextureRepeating(kingPookaMesh->GetTexture3(), true);

	//set kingPooka's hands & staff
	
	leftHand = new OBJMesh("../Meshes/leftHand.obj");
	leftHand->SetTexture(SOIL_load_OGL_texture("../Textures/gold.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(leftHand->GetTexture(), true);
	if (!leftHand->GetTexture())
	{
		return;
	}

	rightHand = new OBJMesh("../Meshes/rightHand.obj");
	rightHand->SetTexture(SOIL_load_OGL_texture("../Textures/gold.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(rightHand->GetTexture(), true);
	if (!rightHand->GetTexture())
	{
		return;
	}

	staff = new OBJMesh("../Meshes/staff.obj");
	staff->SetTexture(SOIL_load_OGL_texture("../Textures/silver.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(staff->GetTexture(), true);
	if (!staff->GetTexture())
	{
		return;
	}

	staffTop = new OBJMesh("../Meshes/staffTop.obj");
	staffTop->SetTexture(SOIL_load_OGL_texture("../Textures/gold.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(staffTop->GetTexture(), true);
	if (!staffTop->GetTexture())
	{
		return;
	}

	predatorMesh = new MD5Mesh();		
	if(!predatorMesh->LoadMD5Mesh("../Meshes/predator.md5mesh")) 
	{
		return;
	}

	healthMesh = new OBJMesh("../Meshes/sphere.obj");
	
	meteor = new OBJMesh("../Meshes/meteor.obj");
	meteor->SetTexture(SOIL_load_OGL_texture("../Textures/stone.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	crystalMesh = Mesh::GenerateCrystal();

	pedestalMesh = Mesh::GeneratePyramid();
	pedestalMesh->SetTexture(SOIL_load_OGL_texture("../Textures/stoneTex2.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	predatorMesh->AddAnim("../Meshes/predWalk.md5anim");
	predatorMesh->AddAnim("../Meshes/predAttack.md5anim");
	predatorMesh->AddAnim("../Meshes/predAttack2.md5anim");
	predatorMesh->AddAnim("../Meshes/predatorDead.md5anim");
	predatorMesh->AddAnim("../Meshes/predMove.md5anim");
	kingPookaMesh->AddAnim("../Meshes/pookaStand.md5anim");
	kingPookaMesh->AddAnim("../Meshes/pookaWalk.md5anim");
}

void Renderer::populateScenenodes()
{	
	cout<<"pop"<<rand()%100<<endl;
	root = new SceneNode();
	kingPooka = new SceneNode();
	kingPooka->SetMesh(kingPookaMesh);
	kingPooka->SetModelScale(Vector3(5,5,5));
	kingPooka->SetBoundingRadius(15.0f);
	kingPooka->SetTransform(Matrix4::Translation(Vector3(500,0,500)));
	kingPooka->objData.setType(KINGPOOKA);
	kingPooka->objData.setState(ALIVE);
	kingPooka->objData.setAnimated(true);
	kingPooka->SetRigidBody(23,engine,500);
	root->AddChild(kingPooka);
	camera->SetDefaultFPHeight(20.0f);
	
	for (int i = 0; i < gameData.getNumberMushrooms(); i++)
	{
		mushroomNode[i] = new SceneNode();
		mushroomNode[i]->SetMesh(mushroom);
		mushroomNode[i]->SetModelScale(Vector3(3.0f, 6.0, 3.0f)*10);
		mushroomNode[i]->SetBoundingRadius(25);
		mushroomNode[i]->SetTransform(Matrix4::Translation(heightMap->returnValidPlace(10.0f)) * Matrix4::Rotation((float)(rand() %360), Vector3(0.0f,1.0f,0.0f)));
		mushroomNode[i]->SetRigidBody(mushroomNode[i]->GetBoundingRadius(), engine,50);
		mushroomNode[i]->objData.setState(ALIVE);
		mushroomNode[i]->objData.setType(MUSHROOM);
		root->AddChild(mushroomNode[i]);	
		lightArray[i] = mushroomNode[i]->GetTransform().GetPositionVector();
	}	
	cout<<"popTest1"<<rand()%100<<endl;
			
	for (int i = 0; i < gameData.getNumberTrees(); i++)
	{
		treeNode[i] = new SceneNode();
		treeNode[i]->SetMesh(tree);
		treeNode[i]->SetModelScale(Vector3(6.0f, (float)(rand() % 10), 6.0f));
		//cout<<"popTest1,1 "<<rand()%100<<endl;
		if (treeNode[i]->GetModelScale().y < 5) 
					treeNode[i]->SetModelScale(Vector3(6,5, 6));
				
		treeNode[i]->SetBoundingRadius(treeNode[i]->GetModelScale().y * 4);
		treeNode[i]->SetTransform(Matrix4::Translation(heightMap->returnValidPlace(treeNode[i]->GetBoundingRadius())) * Matrix4::Rotation((float)(rand() %360), Vector3(0.0f,1.0f,0.0f)));
		treeNode[i]->SetRigidBody(treeNode[i]->GetBoundingRadius(),engine,50);
		treeNode[i]->objData.setState(ALIVE);
		treeNode[i]->objData.setType(TREE);
		root->AddChild(treeNode[i]);
		//cout<<"popTest1,2 "<<rand()%100<<endl;
	}
	cout<<"popTest2"<<rand()%100<<endl;				
	for (int i = 0; i < gameData.getNumberTrees(); i++)
	{
		treeBarkNode[i] = new SceneNode();
		treeBarkNode[i]->SetMesh(treeBark);
		treeBarkNode[i]->SetModelScale(treeNode[i]->GetModelScale());
		treeBarkNode[i]->SetBoundingRadius(treeNode[i]->GetModelScale().y * 10);
		treeBarkNode[i]->SetTransform(treeNode[i]->GetTransform());
		treeBarkNode[i]->objData.setState(ALIVE);
		root->AddChild(treeBarkNode[i]);
	}
	cout<<"popTest3"<<rand()%100<<endl;		
	//flowers 
			
	for (int i = 0; i < gameData.getNumberFlowers(); i++)
	{
		flowerHeadNode[i] = new SceneNode();
		flowerHeadNode[i] ->SetMesh(flowerHead);
		flowerHeadNode[i] ->SetModelScale(Vector3(2,2,2));
		flowerHeadNode[i] ->SetBoundingRadius(15.0f);
		flowerHeadNode[i] ->SetTransform(Matrix4::Translation(Vector3(heightMap->returnValidPlace(flowerHeadNode[i] ->GetBoundingRadius()))) * Matrix4::Translation(Vector3(0,12,0))* Matrix4::Rotation(90,Vector3(1,0,0)));
		flowerHeadNode[i] ->objData.setType(FLOWER);	
		flowerHeadNode[i] ->objData.setState(ALIVE);		
		root->AddChild(flowerHeadNode[i] );
		
		flowerStemNode[i] = new SceneNode();
		flowerStemNode[i] ->SetMesh(flowerStem);
		flowerStemNode[i] ->SetModelScale(Vector3(2,2,2));
		flowerStemNode[i] ->SetBoundingRadius(15.0f);
		flowerStemNode[i] ->SetTransform(flowerHeadNode[i]->GetTransform());			
		flowerStemNode[i] ->objData.setState(ALIVE);			
		root->AddChild(flowerStemNode[i] );
			
		flowerLeavesNode[i] = new SceneNode();
		flowerLeavesNode[i] ->SetMesh(flowerLeaves);
		flowerLeavesNode[i] ->SetModelScale(Vector3(2,2,2));
		flowerLeavesNode[i] ->SetBoundingRadius(15.0f);
		flowerLeavesNode[i] ->SetTransform(flowerHeadNode[i]->GetTransform());			
		flowerLeavesNode[i] ->objData.setState(ALIVE);			
		root->AddChild(flowerLeavesNode[i] );
	}
	cout<<"popTest4"<<rand()%100<<endl;		
	
	for (int i = 0; i < gameData.getNumberStartingPookas(); i++)
		{	
			pookaNode[i] = new Pooka();
			pookaNode[i]->SetMesh(pooka);
			pookaNode[i]->SetModelScale(Vector3(1,1,1));
			pookaNode[i]->SetBoundingRadius(4.0f);
			pookaNode[i]->objData.setType(POOKA);
			pookaNode[i]->objData.setState(INGROUP);
			pookaNode[i]->SetTransform(Matrix4::Translation(Vector3(i*4 + 400.0f, 0.0f ,  400.0f)));
			pookaNode[i]->SetRigidBody(3,engine,10);	
			pookaNode[i]->objData.setHp(100);
			root->AddChild(pookaNode[i]);
			
		}
	

	for(int i = gameData.getNumberStartingPookas(); i < gameData.getNumberStartingPookas() + gameData.getNumberExtraPookas(); i++) 
		{
			pookaNode[i] = new Pooka();
			pookaNode[i]->SetMesh(pooka);
			pookaNode[i]->SetModelScale(Vector3(1,1,1));
			pookaNode[i]->SetBoundingRadius(5.0f);
			pookaNode[i]->objData.setType(POOKA);
			pookaNode[i]->objData.setState(ALIVE);
			pookaNode[i]->SetTransform(Matrix4::Translation(Vector3(heightMap->returnValidPlace(3.0f))));			
			pookaNode[i]->SetRigidBody(3,engine,10);			
			pookaNode[i]->objData.setHp(100);
			root->AddChild(pookaNode[i]);			
		}

	for (int i = 0;i < gameData.getNumberStartingPookas() + gameData.getNumberExtraPookas();i++)
	{
		healthSphere[i] = new SceneNode(healthMesh,Vector4(0.5f,0.0f,1.0f,1.0f));		
		healthSphere[i]->SetModelScale(Vector3(0.1f,0.1f,0.1f));
		healthSphere[i]->objData.setType(HEALTHSPHERE);
		healthSphere[i]->objData.setState(ALIVE);
		healthSphere[i]->SetColour(Vector4(0.5f,0.0f,1.0f,1.0f));
		healthSphere[i]->SetTransform(Matrix4::Translation(Vector3(0,15,0)));			
		pookaNode[i]->AddChild(healthSphere[i]);
	}
	cout<<"popTest5"<<rand()%100<<endl;		

		//set kingPooka's hands & staff

	kingLeft = new SceneNode();
	kingLeft->SetMesh(leftHand);
	kingLeft->SetModelScale(kingPooka->GetModelScale());
	kingLeft->SetBoundingRadius(10.0f);
	kingLeft->SetTransform(Matrix4::Translation(Vector3(-1,5,-5)) * Matrix4::Rotation(180, Vector3(1,1,0)) * Matrix4::Rotation(270, Vector3(0,0,1)));
	kingPooka->AddChild(kingLeft);

	kingRight = new SceneNode();
	kingRight->SetMesh(rightHand);
	kingRight->SetModelScale(kingPooka->GetModelScale());
	kingRight->SetBoundingRadius(10.0f);
	kingRight->SetTransform(Matrix4::Translation(Vector3(1,5,-5)) * Matrix4::Rotation(180, Vector3(1,1,0)) * Matrix4::Rotation(270, Vector3(0,0,1)));
	kingPooka->AddChild(kingRight);

	kingStaff = new SceneNode();
	kingStaff->SetMesh(staff);
	kingStaff->SetModelScale(kingPooka->GetModelScale());
	kingStaff->SetBoundingRadius(10.0);
	kingStaff->SetTransform(Matrix4::Translation(Vector3(0,-2,0)));
	kingRight->AddChild(kingStaff);

	kingStaffTop = new SceneNode();
	kingStaffTop->SetMesh(staffTop);
	kingStaffTop->SetModelScale(kingPooka->GetModelScale());
	kingStaffTop->SetBoundingRadius(10.0);
	kingStaff->AddChild(kingStaffTop);

	for (int i = 0; i < gameData.getNumberPredators(); i++)
	{
		predator[i] = new Predator(i);			
		predator[i]->SetMesh(predatorMesh);
		predator[i]->SetModelScale(Vector3(5,5,5));
		predator[i]->SetBoundingRadius(15.0f);			
		predator[i]->SetTransform(Matrix4::Translation(Vector3(heightMap->returnValidPlace(15))));
		predator[i]->SetRigidBody(15,engine,5);
		predator[i]->objData.setType(PREDATOR);
		predator[i]->objData.setState(ALIVE);
		predator[i]->objData.setAnimated(true);
		predator[i]->SetColour(Vector4(0,1,0,1));
		predator[i]->SetTransform(Matrix4::Translation(heightMap->returnValidPlace(predator[i]->GetBoundingRadius())));
		predator[i]->SetRigidBody(18,engine,1);
		root->AddChild(predator[i]);	
		if(predator[i]->GetRigidBody()>0)
		predatorState[i] = new StateMachine(predator[i], predatorMesh, engine, predator[i]->GetRigidBody());
		else
			cout<<"ERROR PREDATOR"<<endl;
	}	
	cout<<"popTest6"<<rand()%100<<endl;		
	collectables = new SceneNode();
	root->AddChild(collectables);
	crystals = new SceneNode();
	collectables->AddChild(crystals);

	if(heightMap->getRoutePointCount() >= 4)
	{
		for(int i = 0; i < 4; i++)
		{
			crystal[i] = new SceneNode();
			crystal[i]->SetMesh(crystalMesh);
			crystal[i]->SetModelScale(Vector3(6.0f, 30.0f, 6.0f));
			crystal[i]->SetBoundingRadius(10.0f);
			crystal[i]->SetTransform(Matrix4::Translation(Vector3(heightMap->getRoutePoints()[i].x * 16, 5.0f, heightMap->getRoutePoints()[i].y * 16)) * Matrix4::Rotation(45.0f, Vector3(0.0f, 1.0f, 0.0f)) * Matrix4::Rotation(90.0f, Vector3(0.0f, 0.0f, 1.0f))); // * Matrix4::Rotation(yrot, Vector3(0.0f, 1.0f, 0.0f)));
			crystal[i]->objData.setType(CRYSTAL);
			crystal[i]->objData.setState(ALIVE);
			crystal[i]->SetColour(Vector4(0.5f,0,1,1));
			crystals->AddChild(crystal[i]);
			int j = i + gameData.getNumberMushrooms();
			lightArray[j] = crystal[i]->GetTransform().GetPositionVector();
		}


	}
	else
	{
		for(int i = 0; i < heightMap->getRoutePointCount(); i++)
		{
			crystal[i] = new SceneNode();
			crystal[i]->SetMesh(crystalMesh);
			crystal[i]->SetModelScale(Vector3(6.0f, 30.0f, 6.0f));
			crystal[i]->SetBoundingRadius(10.0f);
			crystal[i]->SetTransform(Matrix4::Translation(Vector3(heightMap->getRoutePoints()[i].x * 16, 5.0f, heightMap->getRoutePoints()[i].y * 16)) * Matrix4::Rotation(45.0f, Vector3(0.0f, 1.0f, 0.0f)) * Matrix4::Rotation(90.0f, Vector3(0.0f, 0.0f, 1.0f))); // * Matrix4::Rotation(yrot, Vector3(0.0f, 1.0f, 0.0f)));
			crystal[i]->objData.setType(CRYSTAL);
			crystal[i]->objData.setState(ALIVE);
			crystals->AddChild(crystal[i]);
		}
	}

	SceneNode* objects = new SceneNode();
	root->AddChild(objects);

	Vector3 bp = heightMap->getBaseCoord();
	int r = heightMap->getBaseRadius();

	for(int i = 0; i < 4; i++)
	{
		pedestal[i] = new SceneNode();
		pedestal[i]->SetMesh(pedestalMesh);
		pedestal[i]->SetModelScale(Vector3(5.0f, 20.0f, 5.0f));
		pedestal[i]->SetBoundingRadius(10.0f);
		pedestal[i]->objData.setType(PEDESTAL);
		pedestal[i]->objData.setState(ALIVE);
	}

	pedestal[0]->SetTransform(Matrix4::Translation(Vector3((bp.x + (r * 0.5f)) * 16.0f, 0.0f, bp.z * 16.0f)));
	pedestal[1]->SetTransform(Matrix4::Translation(Vector3(bp.x * 16.0f, 0.0f, (bp.z + (r * 0.5f)) * 16.0f)));
	pedestal[2]->SetTransform(Matrix4::Translation(Vector3((bp.x - (r * 0.5f)) * 16.0f, 0.0f, bp.z * 16.0f)));
	pedestal[3]->SetTransform(Matrix4::Translation(Vector3(bp.x * 16.0f, 0.0f, (bp.z - (r * 0.5f)) * 16.0f)));

	Vector3 tmp = Vector3(0,20,0);
	Vector3 tmp1  = pedestal[0]->GetTransform().GetPositionVector()+tmp;
	Vector3 tmp2 = pedestal[1]->GetTransform().GetPositionVector()+tmp;
	Vector3 tmp3 = pedestal[2]->GetTransform().GetPositionVector()+tmp;
	Vector3 tmp4 = pedestal[3]->GetTransform().GetPositionVector()+tmp;


	cape = new Cloth(25,25,5,tmp3,tmp2,tmp4,tmp1);
		cape->SetTexture(SOIL_load_OGL_texture("../Textures/space5.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
		SetTextureRepeating(cape->GetTexture(), true);

	//cape = new Cloth(25,25,10,Vector3(3513,600,3913),Vector3(3913,600,3513),Vector3(3513,50,3913),Vector3(3913,50,3513));
	//	cape->SetTexture(SOIL_load_OGL_texture("../Textures/flag.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	SetTextureRepeating(cape->GetTexture(), true);

	endPortal[0] = new Spells(    Vector3((bp.x + (r * 0.5f)) * 16.0f, 40.0f, bp.z * 16.0f), Vector3(-1.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, -1.0f), Vector3(-1.0f, 1.0f, 0.0f));
	endPortal[1] = new Spells(    Vector3(bp.x * 16.0f, 40.0f, (bp.z + (r * 0.5f)) * 16.0f), Vector3(1.0f, 0.0f, -1.0f), Vector3(-1.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, -1.0f));
	endPortal[2] = new Spells(    Vector3((bp.x - (r * 0.5f)) * 16.0f, 40.0f, bp.z * 16.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(1.0f, 0.0f, -1.0f), Vector3(1.0f, 1.0f, 0.0f));
	endPortal[3] = new Spells(    Vector3(bp.x * 16.0f, 40.0f, (bp.z - (r * 0.5f)) * 16.0f), Vector3(1.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 1.0f));
	endPortal[4] = new Spells(	  Vector3(bp.x * 16.0f, 195.0f, bp.z  * 16.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
	
	meteorNode = new SceneNode();
	meteorNode->SetMesh(meteor);
	meteorNode->SetTransform(Matrix4::Translation(Vector3(2000,1500,2000)));
	meteorNode->SetModelScale(Vector3(40.0f, 40.0f, 40.0f));
	meteorNode->SetBoundingRadius(30.0f);
	meteorNode->objData.setState(DEAD);
	meteorNode->objData.setType(BOULDER);
	meteorNode->SetRigidBody(40,engine,10000);
	root->AddChild(meteorNode);

	int j = gameData.getNumberMushrooms() + 4;
	lightArray[j] = meteorNode->GetTransform().GetPositionVector();
	
	for(int i = 0; i < 4; i++)
	{
		objects->AddChild(pedestal[i]);
	}

	if(kingPooka->GetRigidBody()>-1)
		//set state machines & animations
	kingState = new StateMachine(kingPooka,kingPookaMesh, engine, kingPooka->GetRigidBody());

	if(playingSound)
		SetupLevelResetSound();


	cout<<"popEnd"<<rand()%100<<endl;
	
}


//***************************************************************************
// ALL BELOW CAN BE MOVED TO WHEREVER WE WANT THE 2D TO 3D PROJECTION TO GO

// for 2d to 3d
 Vector3	Renderer::UnProject(Vector3 position, float aspect, float fov, Camera &c) {
	
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect,fov);
	
	Vector4 clipSpace = Vector4(
		(position.x / (float)width) * 2.0f - 1.0f,
		(position.y / (float)height) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);
	
	Vector4 transformed = invVP * clipSpace;
	
	return Vector3(transformed.x / transformed.w,transformed.y / transformed.w,transformed.z / transformed.w);
}

//inverse proj, 2d to 3d
Matrix4 Renderer::GenerateInverseProjection(float aspect, float fov) {
	Matrix4 m;

	const float h = 1.0f / tan(fov*PI_OVER_360);

	m.values[0]		= aspect / h;
	m.values[5]		= tan(fov*PI_OVER_360);
	m.values[10]	= 0.0f;

	m.values[11]	= -0.5f;
	m.values[14]	= -1.0f;
	m.values[15]	= 0.5f;

	return m;
} 

// generate an inverse view matrix. ( for 2d to 3d
Matrix4 Renderer::GenerateInverseView(Camera &c) {
	float pitcht			=  c.GetPitch();
	float yawt			=  c.GetYaw();
	Vector3 position	=  c.GetPosition();

	Matrix4 iview = 
		Matrix4::Translation(position) *
		Matrix4::Rotation(yawt, Vector3(0,-1,0)) * 
		Matrix4::Rotation(pitcht, Vector3(-1,0,0));

	return iview;
}

//for 2d to 3d
Vector3 Renderer::GetMouseDirectionVector3(float aspect, float fov, Camera &cam) {
	Vector2 mpos = Window::GetMouse()->GetAbsolutePosition();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(mpos.x,
							height - mpos.y,
							0.0f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(mpos.x,
							height - mpos.y,
							0.99999999f
	);
	
	Vector3 a = UnProject(nearPos,aspect,fov,cam);
	Vector3 b = UnProject(farPos,aspect,fov,cam);
	Vector3 c = b-a;
	
	c.Normalise();

	return c;
}

//************************************************************

void Renderer::jumpFunc(int index)
{
		if (pookaJumpArray[index] < 360)
			pookaJumpArray[index]= pookaJumpArray[index] + 0.1f;
	
		else if (pookaJumpArray[index] >= 360)
		{
			pookaJumpArray[index] = 0;
		}

		pookaYPos[index] = (float)(sin((double)pookaJumpArray[index])+ 1) *1.5f; 

		if(pookaNode[index]->objData.getState()!=DEAD)
			pookaNode[index]->SetWorldTransform(Matrix4::Translation(Vector3(0, pookaYPos[index], 0)) * pookaNode[index]->GetWorldTransform() );	
}

void Renderer::handMove(SceneNode* s)
{
	if (s == kingLeft)
	{
		if (leftSinNumber < 360)
		{
			leftSinNumber += 0.3f;
		}
		if (leftSinNumber >=360)
		{
			leftSinNumber = 0;
		}

		leftHandYpos = (float)(sin((double)leftSinNumber) + 1) * 1.5f;
		kingLeft->SetWorldTransform(Matrix4::Translation(Vector3(leftHandYpos,leftHandYpos,leftHandYpos)) * kingLeft->GetWorldTransform());
	}
	else if (s == kingRight)
	{
		if (rightSinNumber < 360)
		{
			rightSinNumber += 0.3f;
		}
		if (rightSinNumber >=360)
		{
			rightSinNumber = 0;
		}

		rightHandYpos = (float)(sin((double)leftSinNumber) + 1) * 1.5f;
		kingRight->SetWorldTransform(Matrix4::Translation(Vector3(rightHandYpos,rightHandYpos,rightHandYpos)) * kingRight->GetWorldTransform());
		kingStaff->SetWorldTransform(Matrix4::Translation(Vector3(rightHandYpos,rightHandYpos,rightHandYpos)) * kingStaff->GetWorldTransform());
		kingStaffTop->SetWorldTransform(Matrix4::Translation(Vector3(rightHandYpos,rightHandYpos,rightHandYpos)) * kingStaffTop->GetWorldTransform());
	}	
}

void Renderer::fireSpell()
{
	if(((int)gameData.getMana() - 50.0f) >= 0)
	{
		float pitcht, yawt;
		Vector3 tmppos;

		if(firstPerson)
		{
			pitcht = camera->GetPitch();
			yawt = camera->GetYaw();
		}
		else
		{
			Matrix4 tmp = kingPooka->GetTransform();
			yawt = tmp.RotationMatrixToYaw();
			tmppos = tmp.GetPositionVector();
			tmppos.y = kingPooka->GetBoundingRadius() - 2;
		}

		pitcht = 0.0f;

		pitcht = pitcht * 3.14159f / 180.0f;
		yawt = yawt * 3.14159f / 180.0f;
		Vector3 tmpdir = Vector3(sin(yawt), -(sin(pitcht)),  -(cos(yawt)));
		spell->SetDirection(Vector3(sin(yawt), -(sin(pitcht)),  -(cos(yawt))));
		spell->SetParticleRate(1.0f);
		spell->SetParticleVariance(0);
		spell->SetParticleSpeed(0.5f);
		spell->SetLaunchParticles(1);
		spell->SetParticleLifetime(1500.0f);
		spell->SetParticleSize(5.0f);
	
		if(firstPerson) spell->setInitialPosition(Vector3(camera->GetPosition().x, camera->GetPosition().y - 5.0f, camera->GetPosition().z));
		else spell->setInitialPosition(tmppos);

		spell->gameObj.setState(ALIVE);
		spell->fireSpell();
		//Sound spell fire
		if(playingSound)
			SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/fire_shot.wav"),SOUNDPRIORITY_HIGH);
		gameData.setMana(gameData.getMana() - 50.0f);
	}
}

void Renderer::drawSpell()
{
	glDepthMask(GL_FALSE);
	
	SetCurrentShader(spellShader);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), spell->GetParticleSize());
	
	Matrix4 oldView = viewMatrix;
	Matrix4 oldProj = projMatrix;
	Matrix4 oldModel = modelMatrix;
	
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(ZNEAR, ZFAR, (float)width / (float)height, FOV);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	spell->Draw();

	viewMatrix = oldView;
	projMatrix = oldProj;
	modelMatrix = oldModel;

	glDepthMask(GL_TRUE);

	glUseProgram(0);
}

void Renderer::drawNexus()
{
	glDepthMask(GL_FALSE);
//	glEnable(GL_BLEND);
	SetCurrentShader(spellShader);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), endPortal[0]->GetParticleSize());

	//modelMatrix = Matrix4::Scale(Vector3(spell->GetParticleSize(), spell->GetParticleSize(), spell->GetParticleSize()));
	Matrix4 oldView = viewMatrix;
	Matrix4 oldProj = projMatrix;
	Matrix4 oldModel = modelMatrix;
	
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(ZNEAR, ZFAR, (float)width / (float)height, FOV);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	if(endScene1)
	{
		for(int i = 0; i < 4; i++)
		{
			endPortal[i]->Draw();
		}
		if(endScene2) endPortal[4]->Draw();
	}

	viewMatrix = oldView;
	projMatrix = oldProj;
	modelMatrix = oldModel;

	glDepthMask(GL_TRUE);
	glUseProgram(0);
}

void Renderer::checkKeyPress(float _msec, float _msPast)
{
	if(!replaying)
	{
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_SPACE) && !spell->isActive())
		{
			ofstream replayFile;
			replayFile.open ("replayFile.txt",ios::app);
			if(replayFile.is_open())
				replayFile<<_msPast<<"_keyS\n";
			replayFile.close();

			spell->setSpell(REDYELLOW);
			fireSpell();
		}
		else if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F6) && !spell->isActive())
		{
			ofstream replayFile;
			replayFile.open ("replayFile.txt",ios::app);
			if(replayFile.is_open())
				replayFile<<_msPast<<"_key6\n";
			replayFile.close();
			spell->setSpell(BLUECYAN);
			fireSpell();
		}
		else if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_F7) && !spell->isActive())
		{
			ofstream replayFile;
			replayFile.open ("replayFile.txt",ios::app);
			if(replayFile.is_open())
				replayFile<<_msPast<<"_key7\n";
			replayFile.close();
			spell->setSpell(GREENPURPLE);
			fireSpell();
		}

	}
	else
	{
		ifstream replayFile ("replayFile.txt");
		string line;
		if (replayFile.is_open())
		{
			bool readNext = true;
			while( (readNext))
			{
				replayFile.seekg(readPos);
				string tmp="";
				getline (replayFile,line);
				if(line=="")
					readNext = false;
				for(int i = 0;i<line.size();i++)
				{
					if(line[i]!= '_')
					{
						tmp += line[i];
					}
					else
					{
						int tmpMsPast = atoi(line.c_str());
						if(tmpMsPast<_msPast)
						{
							if(line[i+4] == 'S')
							{
								spell->setSpell(REDYELLOW);
								fireSpell();
							}
							if(line[i+4] == '6')
							{
								spell->setSpell(BLUECYAN);
								fireSpell();
							}
							if(line[i+4] == '7')
							{
								spell->setSpell(GREENPURPLE);
								fireSpell();
							}
							readPos += line.size() +2;
							break;
						}
						else
						{
							readNext = false;
							break;
						}
					}
				}
			}
		}
	}
}

void Renderer::setWeather()
{
	// Snow
	if(weatherType == 1)
	{
		SetShaderParticle(1.0, weatherType);
		weatherEffect->SetWeatherType(weatherType);
		weatherEffect->SetWeatherColour(Vector4(0.7f, 0.7f, 0.7f, 1.0f));
		weatherEffect->SetParticleVariance(1.0f);
		weatherEffect->SetLaunchParticles(350);
		weatherEffect->SetParticleLifetime(10000.0f);
		weatherEffect->SetParticleSpeed(0.02f);
		weatherEffect->SetParticleRate(95.f);
	}

	// Rain
	if(weatherType == 2)
	{
		
		SetShaderParticle(1.0, weatherType);
		weatherEffect->SetWeatherType(weatherType);
		weatherEffect->SetWeatherColour(Vector4(0.421875f, 0.48046875f, 0.54296875f, 0.8f));
		weatherEffect->SetParticleVariance(0.0f);
		weatherEffect->SetLaunchParticles(1000);
		weatherEffect->SetParticleLifetime(2000.0f);
		weatherEffect->SetParticleSpeed(0.050f);
		weatherEffect->SetParticleRate(100.f);
	}
}

void Renderer::drawWeather()
{
	SetCurrentShader(spellShader);
	Matrix4 oldView = viewMatrix;
	Matrix4 oldProj = projMatrix;
	Matrix4 oldModel = modelMatrix;
	
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(ZNEAR, ZFAR, (float)width / (float)height, FOV);
	modelMatrix.ToIdentity();

	UpdateShaderMatrices();
	weatherEffect->SetWeatherType(weatherType);

	// Weather starter
	weatherStyle();

	setWeather();
	//weatherEffect->Draw();
	viewMatrix = oldView;
	projMatrix = oldProj;
	modelMatrix = oldModel;
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "weatherSelect"), 0);
	glUseProgram(0);
}

void Renderer::weatherStyle()
{
/************************* snow *************************/
	if(weatherSelect == 0) // snow
	{	
		SetCurrentShader(combineShader);
		UpdateShaderMatrices();
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "fogColor"), 1, (float*)&fogColor);
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogOverride"), fogOverride);

		if((difftime(time(0),delayTime) - currTime) > 30 && (difftime(time(0),delayTime) - currTime) < 60)
		{
			weatherType = 1;
			snowCheck = 1;

			if(fogDis > 60)
				fogDis = fogDis - 1.0f;
			
			SetCurrentShader(gBufferShader);
			UpdateShaderMatrices();
			glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogDistance"), fogDis);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "snowCheck"), snowCheck);

			SetCurrentShader(spellShader);
			UpdateShaderMatrices();
		}
	}

/************************* rain *************************/
	else if(weatherSelect == 1) // rain
	{
		SetCurrentShader(combineShader);
		UpdateShaderMatrices();
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "fogColor"), 1, (float*)&fogColor);
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogOverride"), fogOverride);

		if((difftime(time(0),delayTime) - currTime) > 5 && (difftime(time(0),delayTime) - currTime) < 60)
		{
			weatherType = 2;
			weatherRain->SetIsGlobal(true);
			if(playingSound)
				enviromentSound->SetIsGlobal(false);

			if(specVal < 1.8)
				specVal = specVal + 0.005f;

			if(fogDis > 60)
				fogDis = fogDis - 1.0f;

			SetCurrentShader(directionalLightShader);
			UpdateShaderMatrices();
			glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "spec"), specVal);

			SetCurrentShader(gBufferShader);
			UpdateShaderMatrices();
			glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogDistance"), fogDis);

			SetCurrentShader(spellShader);
			UpdateShaderMatrices();
			
		}
	}
	/************************* sunny *************************/
	if(weatherSelect == 2)
	{
		fogDis = 5000.0;
		fogOverride = 0.0f;

		SetCurrentShader(gBufferShader);
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogDistance"), fogDis);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "snowCheck"), snowCheck);
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "snowFactor"), snowFactor);

		SetCurrentShader(combineShader);
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogOverride"), fogOverride);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "fogColor"), 1, (float*)&fogColor);

		SetCurrentShader(spellShader);
		UpdateShaderMatrices();
	}

/************************* night time *************************/
	if(weatherSelect == 3)
	{
		fogDis = 600.0;
		fogColor = Vector3(0.0f, 0.0f, 0.0f);
		fogOverride = 0.85;

		SetCurrentShader(gBufferShader);
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogDistance"), fogDis);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "snowCheck"), snowCheck);
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "snowFactor"), snowFactor);

		SetCurrentShader(combineShader);
		UpdateShaderMatrices();
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "fogOverride"), fogOverride);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "fogColor"), 1, (float*)&fogColor);

		SetCurrentShader(spellShader);
		UpdateShaderMatrices();
	}
}


void Renderer::SetShaderParticle(float f, int w)
{
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "weatherSelect"), w);
}

void Renderer::DetectCrystalPickup()
{
	Vector3 kpp = kingPooka->GetWorldTransform().GetPositionVector();
	float kbr = kingPooka->GetBoundingRadius();

	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); )
	{
		if((*i)->objData.getType() == CRYSTAL)
		{
			Vector3 cp = (*i)->GetWorldTransform().GetPositionVector();
			float br = (*i)->GetBoundingRadius();

			float l = (kpp - cp).Length();
			l *= l;

			float sumr = kbr + br;
			sumr *= sumr;

			if(l < sumr)
			{
				int tmp;

				for(int x = 0; x < 4; x++)
				{
					if(crystal[x] == (*i)) tmp = x;
				}

				gameData.setCrystalsCollected(gameData.getCrystalsCollected() + 1);
				SceneNode::RemoveChild(*i);
				i = nodeList.erase(i);
				crystal[tmp] = NULL;
				if(playingSound)
					SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystal-glass.wav"), SOUNDPRIORITY_HIGH);
			}
			else ++i;
		}
		else ++i;
	}
}

void Renderer::endSceneCheck()
{
	if(gameData.getCrystalsCollected() == 4 && !endScene)
	{
		Vector3 kpp = kingPooka->GetWorldTransform().GetPositionVector();
		kpp.y = 0.0f;

		float kbr = kingPooka->GetBoundingRadius();
		
		Vector3 bp = heightMap->getBaseCoord();
		bp.x = bp.x * 16;
		bp.y = 0.0f;
		bp.z = bp.z * 16;

		float r = (float)heightMap->getBaseRadius() * 16;
		
		float l = (kpp - bp).Length();
		
		l *= l;
			
		float sumr = kbr + r;
		sumr *= sumr;

		if(l < sumr)
		{
			endScene = true;
			
			camera->toggleFirstPerson();
			CameraUpdateF = &Camera::UpdateCamera;
			firstPerson = false;
			camera->SetPosition(Vector3(3410.0f, 400.0f, 3416.0f));
			camera->SetPitch(39.0f);
			camera->SetYaw(138.0f);
		}
	}
}

void Renderer::drawEndScene()
{
	//Stop playing the music and enviroment sound
	if(playingSound)
	{
		enviromentSound->SetIsGlobal(false);
		musicSound->SetIsGlobal(false);
	}
	static int esi = 0;
	static unsigned int t0 = (unsigned int)time(NULL);
	unsigned int t1 = (unsigned int)time(NULL);
	
	if((t1 - t0 >= 2) && esi < 4)

			{
				Vector3 pd = pedestal[esi]->GetWorldTransform().GetPositionVector();
				pd.y = 40.0f;
				crystal[esi] = new SceneNode();
				crystal[esi]->SetMesh(crystalMesh);
				crystal[esi]->SetModelScale(Vector3(6.0f, 30.0f, 6.0f));
				crystal[esi]->SetBoundingRadius(10.0f);
				crystal[esi]->SetTransform(Matrix4::Translation(pd));
				crystal[esi]->objData.setType(CRYSTAL);
				crystal[esi]->objData.setState(ALIVE);
				crystals->AddChild(crystal[esi]);

				esi++;
				t0 = (unsigned int)time(NULL);

			}
	else if((t1 - t0 >= 1) && esi >= 4 && !endScene1)
	{
		if(playingSound)
			SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/portal-long.wav"),SOUNDPRIORITY_HIGH);
		
		for(int i = 0; i < 5; i++)
		{
				endPortal[i]->SetParticleRate(1.0f);
				endPortal[i]->SetParticleVariance(0);
				//endPortal[i]->SetParticleSpeed(0.1f);
				endPortal[i]->SetParticleSpeed(0.05f);
				endPortal[i]->SetLaunchParticles(1);
				//endPortal[i]->SetParticleLifetime(1500.0f);
				endPortal[i]->SetParticleLifetime(3000.0f);
				endPortal[i]->SetParticleSize(5.0f);
		}
		
		endScene1 = true;
		t0 = (unsigned int)time(NULL);
	}
	else if((t1 - t0 >= 3) && endScene1 && !endScene2)
	{
		endScene2 = true;

		endPortal[4]->SetParticleRate(1.0f);
		endPortal[4]->SetParticleVariance(0);
		endPortal[4]->SetParticleSpeed(0.05f);
		endPortal[4]->SetLaunchParticles(1);
		endPortal[4]->SetParticleLifetime(3000.0f);
		endPortal[4]->SetParticleSize(5.0f);
		t0 = (unsigned int)time(NULL);
	}
	
	if((t1 - t0) >= 5 && endScene1 && endScene2)
	{
		esi = 0;
		gameData.setCrystalsCollected(0);
		endSceneOver = true;
		endScene = false;
	}
	
}

void Renderer::SetupSound()
{
	SoundSystem::Initialise();
	
	string soundNames[NUM_SOUNDS] = {
		"../Sounds/awwhowl2.wav",
		"../Sounds/crystal-glass.wav",
		"../Sounds/rain_thunder.wav",
		"../Sounds/bird.wav",
		"../Sounds/zombieattack.wav",
		"../Sounds/summernight-wildlife.wav",
		"../Sounds/pop.wav",
		"../Sounds/apple-crunch.wav",
		"../Sounds/fire_hit.wav",
		"../Sounds/fire_shot.wav",
		"../Sounds/woohoo.wav",
		"../Sounds/whoop.wav",
		"../Sounds/portal-long.wav",
		"../Sounds/headshotp2.wav",
		"../Sounds/woohoo.wav",
		"../Sounds/click.wav",
		"../Sounds/collider_stephen_burns.wav",
		"../Sounds/bamg.wav",
		"../Sounds/pokemon_center.wav",
		"../Sounds/crystalBell.wav"
	};
	
	for(int i = 0; i < NUM_SOUNDS; ++i) 
	{
		Sound::AddSound(soundNames[i]);
	}
	
}

void Renderer::SetupLevelResetSound()
{
	if(kingPooka)
		SoundSystem::GetSoundSystem()->SetListener(kingPooka);

	SoundNode	*s;
	//Adding all the monster areas to hear them
	for (int i = 0; i < gameData.getNumberPredators(); i++)
	{
		float bounding = predator[i]->GetBoundingRadius();
		predSound[i] = new SoundNode();
		predSound[i]->SetSound(Sound::GetSound("../Sounds/awwhowl2.wav")); //awwhowl2 sound
		predSound[i]->SetSoundName("sound0");
		predSound[i]->SetLooping(true);
		predSound[i]->SetRadius(bounding * 25.0f);
		predSound[i]->SetBoundingRadius(bounding * 25.0f);
		predSound[i]->SetTransform(Matrix4::Translation(Vector3(predator[i]->GetWorldTransform().GetPositionVector().x, predator[i]->GetWorldTransform().GetPositionVector().y - 10, predator[i]->GetWorldTransform().GetPositionVector().z )));
		predator[i]->AddChild(predSound[i]);
	}	

	////Add bird sound to the trees
	for (int i = 0; i < gameData.getNumberTrees(); i++)
	{
		float bounding = treeNode[i]->GetBoundingRadius();
		s = new SoundNode();
		s->SetSound(Sound::GetSound("../Sounds/bird.wav")); //birds sound
		s->SetSoundName("sound3");
		s->SetLooping(true);
		s->SetBoundingRadius(bounding * 3.0f);
		s->SetTransform(Matrix4::Translation(Vector3(treeNode[i]->GetWorldTransform().GetPositionVector().x, treeNode[i]->GetWorldTransform().GetPositionVector().y + 20 , treeNode[i]->GetWorldTransform().GetPositionVector().z )));
		treeNode[i]->AddChild(s);
	}

	////Setup rain weather node
	weatherRain = new SoundNode();
	weatherRain->SetSound(Sound::GetSound("../Sounds/rain_thunder.wav"));
	weatherRain->SetLooping(true);
	weatherRain->SetVolume(2.0f);
	weatherRain->SetIsGlobal(false);
	weatherRain->SetPriority(SOUNDPRIORITY_HIGH);
	root->AddChild(weatherRain);

	//Setup the music sound
	musicSound = new SoundNode();
	if(gameData.getLevel() % 2 == 0)
	{
		musicSound->SetSound(Sound::GetSound("../Sounds/pokemon_center.wav"));
	}
	else
	{
		musicSound->SetSound(Sound::GetSound("../Sounds/collider_stephen_burns.wav"));
	}
	musicSound->SetLooping(true);
	musicSound->SetVolume(2.0f);
	musicSound->SetIsGlobal(false);
	musicSound->SetPriority(SOUNDPRIORITY_HIGH);
	root->AddChild(musicSound);

	//Setup the enviroment for the sound
	enviromentSound = new SoundNode();
	enviromentSound->SetSound(Sound::GetSound("../Sounds/summernight-wildlife.wav"));
	enviromentSound->SetLooping(true);
	enviromentSound->SetVolume(2.0f);
	enviromentSound->SetIsGlobal(false);
	enviromentSound->SetPriority(SOUNDPRIORITY_HIGH);
	root->AddChild(enviromentSound);

	enviromentSound->SetIsGlobal(true);
	musicSound->SetIsGlobal(true);

}

void Renderer::RemoveDeadSoundNodes()
{
	if(playingSound)
	{
		for(int i = 0; i < gameData.getNumberPredators(); i++)
		{
			//Remove soundnode from dead preditors
			if(predator[i]->objData.getState() == DEAD && predator[i]->HasChildren() && predSound[i]->GetLooping())
			{
				predSound[i]->SetLooping(false);
				SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/headshotp2.wav"),SOUNDPRIORITY_HIGH);
				preditorKilled++;
			}	
		}
	}
}

void Renderer::toggleSound()
{
	if(musicSound->GetIsGlobal())
	{
		musicSound->SetIsGlobal(false);
		enviromentSound->SetIsGlobal(false);
	}
	else
	{
		musicSound->SetIsGlobal(true);
		enviromentSound->SetIsGlobal(true);
	}
}

void Renderer::tutorialFunc()
{
	SetCurrentShader(hudShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	viewMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,0.5f)) * Matrix4::Scale(Vector3((float)width/4,(float)height/4,1));// * Matrix4::Rotation(180,(Vector3(0,1,0)))  * Matrix4::Rotation(270,(Vector3(0,0,1)));
	UpdateShaderMatrices();
	
	if(openingTutFlag)
	{
		tutorialFlag = true;
		if(!pause)
		{
			pause = true;
			engine->togglePause();
		}
		tutorialQuad->Draw();

		if(crystalBell)
		{
			crystalBell = false;
			if(playingSound) //If sound is switched on
				SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystalBell.wav"),SOUNDPRIORITY_HIGH);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RETURN))
		{
			engine->togglePause();
			pause = false;
			openingTutFlag = false;
			flowerTutFlag = true;
			pookaTutFlag = true;
			crystalTutFlag = true;
			predatorTutFlag = true;
			tutorialFlag = false;
			meteorTutFlag = false;
			crystalBell = true;
		}		
	} //End of opening tut flag

	if (flowerTutFlag)
	{
		for ( int i = 0; i < gameData.getNumberFlowers(); i++)
			if ( (kingPooka->GetWorldTransform().GetPositionVector() - flowerStemNode[i]->GetWorldTransform().GetPositionVector()).Length() < 100)
			{
				tutorialFlag = true;
				if(!pause)
					{
						pause = true;
						engine->togglePause();
					}
							tutFlowerQuad->Draw();

				if(crystalBell)
				{
					crystalBell = false;
					if(playingSound) //If sound is switched on
						SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystalBell.wav"),SOUNDPRIORITY_HIGH);
				}

				if (Window::GetKeyboard()->KeyDown(KEYBOARD_RETURN))
				{
					engine->togglePause();
					pause = false;			
					flowerTutFlag = false;
					tutorialFlag = false;
					crystalBell = true;
				}			
			}
	} //End of flower tut flag

	if (crystalTutFlag)
	{
		for ( int i = 0; i < 4; i++)
			if ((kingPooka->GetWorldTransform().GetPositionVector() - crystal[i]->GetWorldTransform().GetPositionVector()).Length() < 100)
			{
				tutorialFlag = true;
				if(!pause)
				{
					pause = true;
					engine->togglePause();
				}
				tutCrystalQuad->Draw();

				if(crystalBell)
				{
					crystalBell = false;
					if(playingSound) //If sound is switched on
						SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystalBell.wav"),SOUNDPRIORITY_HIGH);
				}

				if (Window::GetKeyboard()->KeyDown(KEYBOARD_RETURN))
				{
					engine->togglePause();
					pause = false;			
					crystalTutFlag = false;
					tutorialFlag = false;
					crystalBell = true;
				}			
			}
	} //End of crystal tut flag

	if (pookaTutFlag)
	{
		for ( int i = gameData.getNumberStartingPookas(); i < gameData.getNumberStartingPookas() + gameData.getNumberExtraPookas(); i++)
			if ((kingPooka->GetWorldTransform().GetPositionVector() - pookaNode[i]->GetWorldTransform().GetPositionVector()).Length() < 100)
			{
				tutorialFlag = true;

				if(!pause)
					{
						pause = true;
						engine->togglePause();
					}
				tutPookaQuad->Draw();
					
				if(crystalBell)
				{
					crystalBell = false;
					if(playingSound) //If sound is switched on
						SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystalBell.wav"),SOUNDPRIORITY_HIGH);
				}

				if (Window::GetKeyboard()->KeyDown(KEYBOARD_RETURN))
				{
					engine->togglePause();
					tutorialFlag = false;
					pause = false;			
					pookaTutFlag = false;
					crystalBell = true;
				}			
			}
	} //End of pooka tut flag

	if (predatorTutFlag)
	{
		for ( int i = 0; i < gameData.getNumberPredators(); i++)
			if ((kingPooka->GetWorldTransform().GetPositionVector() - predator[i]->GetWorldTransform().GetPositionVector()).Length() < 200)
			{
				tutorialFlag = true;
				if(!pause)
					{
						pause = true;
						engine->togglePause();
					}
				tutPredatorQuad->Draw();

				if(crystalBell)
				{	
					crystalBell = false;
					if(playingSound) //If sound is switched on
						SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystalBell.wav"),SOUNDPRIORITY_HIGH);
				}

				if (Window::GetKeyboard()->KeyDown(KEYBOARD_RETURN))
				{
					engine->togglePause();
					tutorialFlag = false;
					pause = false;			
					predatorTutFlag = false;
					crystalBell = true;
				}			
			}	
	} //End of predator flag

	if (meteorTutFlag)
	{
		tutorialFlag = true;
		if(!pause)
		{
			pause = true;
			engine->togglePause();
		}
		tutMeteorQuad->Draw();
		if(crystalBell)
		{
			crystalBell = false;
			if(playingSound)	//If sound is switched on
				SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/crystalBell.wav"),SOUNDPRIORITY_HIGH);
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RETURN))
		{
			tutorialFlag = false;
			pause = false;			
			meteorTutFlag = false;
			engine->togglePause();
			crystalBell = true;
		}			
	} //End of meteor flag

	setPerspectiveProjection();	
}

void Renderer::endGame()
{
	projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, float(width), 0.0f, float (height), 0.0f);
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/4.0f,0.99f)) * Matrix4::Scale(Vector3((float)width/2,(float)height/2,1));
	UpdateShaderMatrices();
	if (pause != true)
		endGameClock = time(0);
	
	pause = true;
	endGameQuad->Draw();
	setPerspectiveProjection();	
	if (difftime(time(0),endGameClock)>= 10)
	{		
		endGameFlag = true;
	}
}

void Renderer::pauseScreen()
{
	if (gameData.getNumberOfPookas()-1 != 0)
	{

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
		modelMatrix.ToIdentity();
		viewMatrix.ToIdentity();

		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	
		modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height/2,0.05f)) * Matrix4::Scale(Vector3((float)width/2,(float)height/2,1));
		UpdateShaderMatrices();			

		pauseQuad->Draw();
		glDisable(GL_BLEND);

		modelMatrix.ToIdentity();
		modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height-400,0.1f)) * Matrix4::Scale(Vector3((float)width/12,(float)height/20,1));
		UpdateShaderMatrices();
		pauseContinue->Draw();
		modelMatrix.ToIdentity();
		modelMatrix = Matrix4::Translation(Vector3((float)width/2,(float)height-500,0.1f)) * Matrix4::Scale(Vector3((float)width/12,(float)height/20,1));
		UpdateShaderMatrices();
		pauseExit->Draw();
		
		glEnable(GL_BLEND);
	
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float) width/ (float) height, 45.0f);
		UpdateShaderMatrices();
	}

}

void Renderer::togglePause()
{	
	if(musicSound->GetIsGlobal())
	{
		musicSound->SetIsGlobal(false);
		enviromentSound->SetIsGlobal(false);
		pauseCounter = 1;
	}
	else
	{
		musicSound->SetIsGlobal(true);
		enviromentSound->SetIsGlobal(true);
		realTogglePause();
	}	
	
}

void Renderer::realTogglePause()
{
	if (!tutorialFlag)
	{
		pause = !pause;
		engine->togglePause();
		lastTime = time(0);
		pauseCounter = 0;
	}
}

void Renderer::checkBounds(Vector2 mousePos)
{
	Vector2 exitGamePos = Vector2((float)width/2,(float)500);
	Vector2 continueGamePos = Vector2((float)width/2,(float)400);

	
	if (( mousePos- exitGamePos ).Length() < 50)
	{
		if (Window::GetMouse()->ButtonTriggered(MOUSE_LEFT))		
		fullGameOver = true;
	}

	if (( mousePos- continueGamePos ).Length() < 50)
	{
		if (Window::GetMouse()->ButtonTriggered(MOUSE_LEFT))
		{
			togglePause();
		}
				
	}

}

void Renderer::mouseFPCheck(float msec)
{
	if(!replaying)
	{
		float tmp_x = Window::GetMouse()->GetRelativePosition().x;
		float tmp_y = Window::GetMouse()->GetRelativePosition().y;
		if(tmp_x !=0||tmp_y!=0)
		{
			ofstream msreplayFile;
			msreplayFile.open ("msReplay.txt",ios::app);
			if(msreplayFile.is_open())
				msreplayFile<<msPast<<"_y_"<<Window::GetMouse()->GetRelativePosition().y<<"_x_"<<Window::GetMouse()->GetRelativePosition().x<<"_ms_"<<msec<<"\n";
			msreplayFile.close();

			pitch += tmp_y*(msec/15);

			yaw += (tmp_x)*(msec/15);
		}
	}
	else
	{
		float tmp_x = 0;
		float tmp_y = 0;
		int readMS =0;
		int prevReadMS =0;
		string line;
		string line2;
		ifstream msreplayFile ("msReplay.txt");
		ifstream kbreplayFile ("kbReplay.txt");
		if (msreplayFile.is_open()&&kbreplayFile.is_open())
		{
			bool readNext = true;
			while( (readNext))
			{
				tmp_x = 0;
				tmp_y = 0;
				msreplayFile.seekg(msReadPos);
				string tmp="";
				getline (msreplayFile,line);
				int counter=0;
				bool neg = false;
				for(int i =0;i <line.length();i++){
					if (counter ==0)
					{
						if(line[i]!='_')
						{
							tmp+=line[i];
						}
						else
						{
							prevReadMS = readMS;
							readMS = atoi(tmp.c_str());
							//read Keyboard
							bool readingkb = true;
							while(readingkb)
							{
								kbreplayFile.seekg(kbReadPos);
								string tmp2="";
								getline (kbreplayFile,line2);
								if(line2=="")
									readingkb = false;
								for(int j =0;j <line2.length();j++){
									if(line2[j]=='s')
									{
										readingkb=false;
										break;
									}
									if(line2[j]!='_')
									{
										tmp2+=line2[j];
									}
									else
									{
										int kbreadMS = atoi(tmp2.c_str());
										if(kbreadMS<=msPast)
										{
											if(kbreadMS<readMS&&kbreadMS>=prevReadMS)
											{
												tmp2="";
												for(int k =j+1;k <line2.length();k++)
												{
													if(line2[k]!='_')
													{
														tmp2+=line2[k];
													}
													else
													{
														int tmpmsec = atoi(tmp2.c_str());
														cout<<tmpmsec<<endl;
														//cout<<msec<<"_";
														tmp2= line2[k+3];
														//cout<<tmp2<<endl;
														if(line2[k+3] == 'W')
														{
															Vector3 tmppos = Vector3(0,0,0);
															if(engine->getBody(kingPooka->GetRigidBody()))
																tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

															Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
																Matrix4::Translation(-tmppos);

															Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
															kingPooka->addVelocity(dir * tmpmsec/10,0);
														}
														else if(line2[k+3] == 'S')
														{
															Vector3 tmppos = Vector3(0,0,0);
															if(engine->getBody(kingPooka->GetRigidBody()))
																tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

															Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
																Matrix4::Translation(-tmppos);

															Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
															kingPooka->addVelocity(-dir * tmpmsec/10,0);
														}
														else if(line2[k+3] == 'A')
														{
															Vector3 tmppos = Vector3(0,0,0);
															if(engine->getBody(kingPooka->GetRigidBody()))
																tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

															Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
																Matrix4::Translation(-tmppos);

															Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
															Vector3 strafe = Vector3::Cross(dir,Vector3(0,1,0));
															kingPooka->addVelocity(-strafe * tmpmsec/10,0);
														}
														else if(line2[k+3] == 'D')
														{
															Vector3 tmppos = Vector3(0,0,0);
															if(engine->getBody(kingPooka->GetRigidBody()))
																tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

															Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
																Matrix4::Translation(-tmppos);

															Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
															Vector3 strafe = Vector3::Cross(dir,Vector3(0,1,0));
															kingPooka->addVelocity(strafe * tmpmsec/10,0);
														}
														else
															readingkb = false;
														kbReadPos += k+6;
													}
												}
											}
											else 
												readingkb = false;
										}
										else
											readingkb = false;
									}
								}
							}
							//finished reading Keyboard
							if(readMS<=msPast)
							{
								tmp="";
								i+=2;
								counter ++;
							}
							else
							{
								readNext = false;
								break;
							}
						}
					}
					else if(counter==1)
					{
						if(line[i]=='-')
						{
							neg = true;
						}
						else if(line[i]!='_')
						{
							tmp+=line[i];
						}
						else
						{
							tmp_y = atof(tmp.c_str());
							if(neg)tmp_y = -tmp_y;
							tmp="";
							i+=2;
							counter ++;
							neg = false;
						}
					}
					else if(counter==2)
					{
						if(line[i]=='-')
						{
							neg = true;
						}
						else if(line[i]!='_')
						{
							tmp+=line[i];
						}
						else
						{
							tmp_x = atof(tmp.c_str());
							if(neg)tmp_x = -tmp_x;
							tmp ="";
							for(int l = i+4; l<line.size()-1;l++)
							{
								tmp+= line[l];
							}
							float tmpMsec = atof(tmp.c_str());
							msReadPos += i+13;

							pitch += tmp_y*2*(tmpMsec/15);
							yaw += (tmp_x)*(tmpMsec/15);

							break;
						}
					}
				}
			}
		msreplayFile.close();
		}
	}
	//else
	//{
	//	float tmp_x = 0;
	//	float tmp_y = 0;
	//	int readMS =0;
	//	int prevReadMS =0;
	//	string line;
	//	ifstream msreplayFile ("msReplay.txt");
	//	if (msreplayFile.is_open())
	//	{
	//		bool readNext = true;
	//		while( (readNext))
	//		{
	//			tmp_x = 0;
	//			tmp_y = 0;
	//			msreplayFile.seekg(msReadPos);
	//			string tmp="";
	//			getline (msreplayFile,line);
	//			int counter=0;
	//			bool neg = false;
	//			for(int i =0;i <line.length();i++){
	//				if (counter ==0)
	//				{
	//					if(line[i]!='_')
	//					{
	//						tmp+=line[i];
	//					}
	//					else
	//					{
	//						prevReadMS = readMS;
	//						readMS = atoi(tmp.c_str());
	//						//read Keyboard
	//						bool readingkb = true;
	//						if(readMS<=msPast)
	//						{
	//							tmp="";
	//							i+=2;
	//							counter ++;
	//						}
	//						else
	//						{
	//							readNext = false;
	//							break;
	//						}
	//					}
	//				}
	//				else if(counter==1)
	//				{
	//					if(line[i]=='-')
	//					{
	//						neg = true;
	//					}
	//					else if(line[i]!='_')
	//					{
	//						tmp+=line[i];
	//					}
	//					else
	//					{
	//						tmp_y = atof(tmp.c_str());
	//						if(neg)tmp_y = -tmp_y;
	//						tmp="";
	//						i+=2;
	//						counter ++;
	//						neg = false;
	//					}
	//				}
	//				else if(counter==2)
	//				{
	//					if(line[i]=='-')
	//					{
	//						neg = true;
	//					}
	//					else if(line[i]!='_')
	//					{
	//						tmp+=line[i];
	//					}
	//					else
	//					{
	//						tmp_x = atof(tmp.c_str());
	//						if(neg)tmp_x = -tmp_x;
	//						tmp ="";
	//						for(int l = i+4; l<line.size()-1;l++)
	//						{
	//							tmp+= line[l];
	//						}
	//						float tmpMsec = atof(tmp.c_str());
	//						msReadPos += i+13;

	//						yaw += (tmp_x)*(tmpMsec/15);

	//						break;
	//					}
	//				}
	//			}
	//		}
	//	msreplayFile.close();
	//	}
	//}
	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}

	Vector3 tmppos = Vector3(0,0,0);
	if(engine->getBody(kingPooka->GetRigidBody()))
		tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();
	tmppos.y = kingPooka->GetBoundingRadius();
		
	Matrix4 tmp = Matrix4::Translation(tmppos) *
		Matrix4::Rotation(yaw, Vector3(0, -1, 0));
	tmp.values[13] = 0.0f;

	kingPooka->SetTransform(tmp);

	if(fpReplay || !replaying)
	{
		camera->SetPosition(tmppos);
		camera->SetYaw(tmp.RotationMatrixToYaw());
		camera->SetPitch(pitch);
		//CameraUpdateF = &Camera::UpdateCameraFP;
		kingPooka->setFPS(true);
		Window::GetMouse()->SetAbsolutePosition(width/2, height/2);
	}

	if(!replaying)
	{
			if ( Window::GetKeyboard()->KeyDown(KEYBOARD_W) )
			{
				ofstream kbReplayFile;
				kbReplayFile.open ("kbReplay.txt",ios::app);
				if(kbReplayFile.is_open())
					kbReplayFile<<msPast<<"_"<<msec<< "_kbW\n";
				kbReplayFile.close();

				Vector3 tmppos = Vector3(0,0,0);
				if(engine->getBody(kingPooka->GetRigidBody()))
					tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

				Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
					Matrix4::Translation(-tmppos);

				Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
				kingPooka->addVelocity(dir * msec/10,0);
			}
			else if ( Window::GetKeyboard()->KeyDown(KEYBOARD_S) )
			{
				ofstream kbReplayFile;
				kbReplayFile.open ("kbReplay.txt",ios::app);
				if(kbReplayFile.is_open())
					kbReplayFile<<msPast<<"_"<<msec<< "_kbS\n";
				kbReplayFile.close();

				Vector3 tmppos = Vector3(0,0,0);
				if(engine->getBody(kingPooka->GetRigidBody()))
					tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

				Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
					Matrix4::Translation(-tmppos);

				Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
				kingPooka->addVelocity(-dir * msec/10,0);
			}
			else if ( Window::GetKeyboard()->KeyDown(KEYBOARD_A) )
			{

				ofstream kbReplayFile;
				kbReplayFile.open ("kbReplay.txt",ios::app);
				if(kbReplayFile.is_open())
					kbReplayFile<<msPast<<"_"<<msec<< "_kbA\n";
				kbReplayFile.close();

				Vector3 tmppos = Vector3(0,0,0);
				if(engine->getBody(kingPooka->GetRigidBody()))
					tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

				Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
					Matrix4::Translation(-tmppos);

				Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
				Vector3 strafe = Vector3::Cross(dir,Vector3(0,1,0));
				kingPooka->addVelocity(-strafe * msec/10,0);
			}
			else if ( Window::GetKeyboard()->KeyDown(KEYBOARD_D) )
			{
				ofstream kbReplayFile;
				kbReplayFile.open ("kbReplay.txt",ios::app);
				if(kbReplayFile.is_open())
					kbReplayFile<<msPast<<"_"<<msec<< "_kbD\n";
				kbReplayFile.close();

				Vector3 tmppos = Vector3(0,0,0);
				if(engine->getBody(kingPooka->GetRigidBody()))
					tmppos = engine->getBody(kingPooka->GetRigidBody())->GetPosition();

				Matrix4 tmp = Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
					Matrix4::Translation(-tmppos);

				Vector3 dir= Vector3(tmp.values[3],tmp.values[7],tmp.values[11])-Vector3(tmp.values[2],tmp.values[6],tmp.values[10]);
				Vector3 strafe = Vector3::Cross(dir,Vector3(0,1,0));
				kingPooka->addVelocity(strafe * msec/10,0);
			}
	}
}