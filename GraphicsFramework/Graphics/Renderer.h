#pragma once

#include "../Framework/OGLRenderer.h"
#include "../Framework/Camera.h"
#include "../Framework/minimapCamera.h"
#include "../Framework/HeightMap.h"
#include "../Framework/OBJMesh.h"
#include "../Framework/MD5Mesh.h"
#include "../Framework/Ray.h"
#include "../Framework/Frustum.h"
#include "../Framework/PhysicsThread.h"
#include "../Framework/DebugDraw3D.h"
#include "../Framework/AStarThread.h"
#include "../Framework/TextMesh.h"
#include "../Framework/Cloth.h"
#include "../Graphics/Spells.h"
#include "../Graphics/WeatherEmitter.h"
#include "../Framework/StateMachine.h"
#include "../Framework/GameObjectTypes.h"
#include "../Framework/Enemy.h"
#include "../Framework/GameData.h"
#include "../Framework/Octree.h"
#include "../Framework/SoundSystem.h"
#include "../Framework/CameraNode.h"
#include "../Framework/Pooka.h"
#include "../Framework/XmlParser.h"
#include "../Framework/Types.h"

#include <time.h>
#include <algorithm>

// Texture units used:

// Texture Unit 0:  diffuseTex		(Texture mapping)
// Texture Unit 1:  bumpTex			(Bump mapping)
// Texture Unit 2:  cubeTex			(Cube mapping)
// Texture Unit 3:  shadowTex		(Shadow mapping)
// Texture Unit 4:  furTex			(Shell texturing)

// Deferred rendering:
// Texture Unit 5:  G_DepthTex
// Texture Unit 6:  G_ColourTex
// Texture Unit 7:  G_NormTex
// Texture Unit 8:  G_MaterialTex
// Texture Unit 9:  L_EmissiveTex
// Texture Unit 10: L_SpecularTex

// Texture Unit 11: diffuseTex2		(Texture mapping)
// Texture Unit 12: diffuseTex3		(Texture mapping)
// Texture Unit 13: bumpTex2		(Bump mapping)
// Texture Unit 14: bumpTex3		(Bump mapping)


// Perspective view
#define ZNEAR			1
#define ZFAR			5000
#define FOV				45

#define SHADOWSIZE		2048 * 2

#define LIGHTNUM		10
#define NUM_SOUNDS		20

#define	SOUND_DEBUG		0

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void SetCurrentTime(float ct) { currentTime = ct; }
	void SetFPS(unsigned int f) { fps = f; }
	inline void setPause(bool a) {pause = a;}
	void toggleViewMode();
	void toggleFreeView();
	void toggleBumpmapping();
	void togglePause();
	void realTogglePause();	//Bit of a hack to get sound to pause :)
	int  pauseCounter;		
	
	void endGame();
	void toggleDebug();
	void toggleFirstPerson();
	void toggleFrames();
	void toggleSound(); 
	void setEndSceneOver(bool a) {endSceneOver = a;}
	bool getEndGame(){return endGameFlag;}
	void setEndGame(bool a) {endGameFlag = a;}
	bool getGameOver() {return fullGameOver;}
	Vector3 GetMouseDirectionVector3(float aspect, float fov, Camera &c);

	void setReplay(){replaying = true;resetReplay=true;}

	int getMsPast(){return msPast;}

protected:

	void levelReset();
	void cleanNodes();

	// Draw functions
	void drawSkybox();						// To draw the skybox

	void drawShadowScene();					// Fills the depth buffer rendering from the light's perspective
	
	void fillBuffers();						// G-Buffer Filling Pass
	void lightingPass();					// Lighing Accumulation Pass
	void drawPointLights();					// Point Lights Rendering
	void drawDirectionalLight();			// Directional Light Rendering
	void combineBuffers();					// Combination Pass

	void drawHeightmap();					// Renders the terrain

	void drawMinimap();						// Renders terrain for the minimap
	void drawHUD();							// Renders the HUD

	void drawGrass();						// Renders grass on the terrain
	void drawCape();

	void drawNexus();
	void drawSpell();
	void fireSpell();
	void drawEndScene();

	void drawWeather();				// Renders the weather effects
	
	void drawDebug();
	void SetupSound();
	void SetupLevelResetSound();

	// Helper functions
	void setPerspectiveProjection();								// Sets perspective projection
	void generateScreenTexture(GLuint &into, bool depth = false);	// Make a new texture
	void drawText(TextMesh *textMesh, 
				  const string &text, 
				  const Vector3 &position, 
				  const float size = 10.0f);// Renders text to screen
	void projCheck();						// Checks something (projection?)
	void makeMeshes();						// just to keep constructor clean
	void populateScenenodes();
	void jumpFunc(int index);
	void checkKeyPress(float msec, float msPast);
	void setWeather();						// Sets up the data for the weather effects
	void handMove(SceneNode* s);
	void tutorialFunc();
	void pauseScreen();

	void mouseFPCheck(float msec);

	// Scene node Functions
	vector<SceneNode*> nodeList;				// Opaque objects node list
	vector <SceneNode*> transparentNodeList;	// Transparent objects node list
	void BuildNodeLists(SceneNode* from);		// Builds both node lists
	void SortNodeLists();						// Sorts both node lists (front to back / back to front)
	void DrawNode(SceneNode*n);					// Recursive function to render all the nodes in the scene node
	void DrawNodes();							// Calls the recursive function to render nodes on both node lists
	void ClearNodeLists();						// Clears both node lists
	void DetectCrystalPickup();
	void endSceneCheck();

	void (Camera::*CameraUpdateF)(int,float, Vector3);
	void SetShaderParticle(float f, int w);
	void checkBounds(Vector2 mousePos);
	void weatherStyle();

	void RemoveDeadSoundNodes();
	
	//replay
	ofstream replayFile;
	int msPast;
	bool resetReplay;
	bool replaying;
	int readPos;
	int tacReadPos;
	int msReadPos;
	int kbReadPos;
	int viewReadPos;
	bool untoggle;
	bool fpReplay;


	// Shaders
	Shader *skyboxShader;					// Shader to sample cube map

	Shader *shadowShader;					// Shader to fill our Depth buffer

	Shader *gBufferShader;					// Shader to fill our GBuffers
	Shader *directionalLightShader;			// Shader to calculate lighting for directional light
	Shader *pointLightShader;				// Shader to calculate lighting for point lights
	Shader *combineShader;					// shader to stick it all together
	Shader *minimapShader;					// Shader to draw the minimap
	Shader *hudShader;						// Shader to draw the HUD
	Shader *cursorShader;					// Shader to draw the cursor on screen
	Shader *furShader;						// Shader to draw the grass on the terrain
	Shader *spellShader;
	Shader *debugShader;

	//Frustum & SceneNodes
	SceneNode* root;
	Frustum frameFrustum;
	Pooka* pookaNode[200];
	SceneNode* kingPooka;
	SceneNode* kingLeft;					//hands
	SceneNode* kingRight;
	SceneNode* kingStaff;
	SceneNode* kingStaffTop;
	Predator* predator[20];
	SceneNode* crystal[4];
	SceneNode* pedestal[4];
	SceneNode* crystals;
	SceneNode* healthSphere[200];
	SceneNode* collectables;
	SceneNode* meteorNode;
	SceneNode* flowerHeadNode[50];
	SceneNode* flowerLeavesNode[50];
	SceneNode* flowerStemNode[50];
	SceneNode* treeNode[50];
	SceneNode* mushroomNode[50];
	SceneNode* treeBarkNode[50];
	//Sound nodes
	SoundNode* predSound[20];
	SoundNode* weatherRain;
	SoundNode* musicSound;
	SoundNode* enviromentSound;

	// Meshes
	Mesh *quad;	
	OBJMesh* meteor;

	Mesh* tutorialQuad;
	Mesh* tutFlowerQuad;
	Mesh* tutPredatorQuad;
	Mesh* tutPookaQuad;
	Mesh* tutCrystalQuad;
	// To draw a full-screen quad
	OBJMesh *lightSphere;					// Light volume
	HeightMap *heightMap;					// Terrain
	Mesh *avatarMesh;						// Avatar mesh for minimap
	Mesh* triangle;
	Mesh* pauseQuad;
	Mesh* pauseExit;
	Mesh* pauseContinue;
	Mesh* crystalMesh;
	Mesh* pedestalMesh;
	Mesh* bar1;
	Mesh* crosshair;
	Mesh* bar2;
	Mesh* tutMeteorQuad;
	Mesh* endGameQuad;
	MD5Mesh* pooka;
	MD5Mesh* predatorMesh;

	MD5Mesh* kingPookaMesh;
	OBJMesh* circle;
	OBJMesh* arrow;
	OBJMesh* mushroom;
	OBJMesh* leftHand;						//hands for kingPooka
	OBJMesh* rightHand;
	OBJMesh* staff;
	OBJMesh*staffTop;
	OBJMesh* flowerHead;
	OBJMesh* flowerStem;
	OBJMesh* tree;
	OBJMesh* healthMesh;
	OBJMesh* treeBark;
	OBJMesh* flowerLeaves;
	Cloth* cape;
	Spells* spell;
	Spells* endPortal[5];
	WeatherEmitter* weatherEffect;

	//State Machines
	StateMachine* kingState;
	StateMachine* predatorState[20];

	// Cursor texture
	GLuint cursorTex;


	// HUD
	Font *basicFont;						// Basic font for text
	TextMesh *textMeshScore;
	TextMesh *textMeshLevel;
	TextMesh *textMeshCreatures;
	TextMesh *textMeshCrystals;
	TextMesh *textMeshFrames;
	GLuint textureCreature;					// Keeps a texture for creature display on HUD
	GLuint textureCrystal;					// Keeps a texture for crystal count display on HUD

	//Cube mapping
	GLuint cubeMap;							// Store our cube map


	// Shadow mapping
	GLuint shadowTex;						// Store shadow texture
	GLuint shadowFBO;						// Store Depth data
	Matrix4 shadowMatrix;					// Store shadow projection matrix


	// Deferred rendering
	GLuint G_FBO;							// FBO for our G-Buffer pass
	GLuint G_ColourTex;						// Albedo goes here
	GLuint G_NormalTex;						// Normals go here
	GLuint G_DepthTex;						// Depth goes here
	GLuint G_MaterialTex;					// Different materials go here

	GLuint L_FBO;							// FBO for our lighting pass
	GLuint L_EmissiveTex;					// Store emissive lighting
	GLuint L_SpecularTex;					// Store specular lighting


	// Minimap buffer & texture
	GLuint minimapTex;						// Store minimap texture
	GLuint minimapFBO;						// Minimap buffer


	// Camera & light
	Camera *camera;							// Our usual camera
	minimapCamera *minimapCam;				// Camera for the minimap
	Light *light;							// Main light
	Light *pointLights; 					// Array of point lights
	unsigned int lightsNumber;				// Number of point lights


	// Other variables
	float oldMsec;
	float heightMapMaxHeight;				// Keeps the height map max height
	float minimapDimension;					// Keeps the dimension of the minimap
	bool bumpMapping;						// Flag to activate / deactivate Cel shading
	bool rightClick;
	bool leftClick; 
	int  meteorTime;
	int nodeNumber;		
	float  leftSinNumber;
	float rightSinNumber;
	bool debugEnabled;
	bool firstPerson;
	bool endGameFlag;
	float pookaJumpArray[200];					//used for jumping
	float pookaYPos[200];
	float leftHandYpos;
	float rightHandYpos;
	float msec;
	bool firstPass;
	bool endScene;
	bool endScene1;
	bool endScene2;
	bool hitPredator;
	int totalScore;
	bool hitFlower;
	float crystalRotation;
	float currentTime;
	int fps;
	bool fullGameOver;
	bool eatFlower;
	bool endSceneOver;
	
	bool openingTutFlag;
	bool tutorialFlag;
	bool flowerTutFlag;
	bool predatorTutFlag;
	bool pookaTutFlag;
	bool meteorTutFlag;
	bool crystalTutFlag;
	bool frames;
	bool gameOverFlag;
	float lastTime;
	clock_t delayTime;

	//weather variables
	int weatherSelect;
	int currTime;
	int snowCheck;
	float snowFactor;
	float fogOverride;
	Vector3 fogColor;
	float specVal;
	float fogDis;
	int weatherType;

	//Data Driven Part
	GameData gameData;	
	XmlParser myParser;

	clock_t flowerClock;
	clock_t endGameClock;
	int flowerCount;
	bool delHNode;
	bool delLNode;
	bool delSNode;

	Window* win;


	//Vector3
	Vector3		UnProject(Vector3 position, float aspect, float fov, Camera &c);
	Vector3		destination; //set by right click
	Vector3		arrowPos;	
	Vector3		circlePos;  //where to draw circle
	Vector3		lightArray[50];
	//Matrix4
	Matrix4		GenerateInverseProjection(float aspect, float fov);
	Matrix4		GenerateInverseView(Camera &c);

	PhysicsThread *engine;
	int msecCounter;

	AStarThread* AstarThread;
	bool pause;

	//sound
	CameraNode		*cameraNode;
	short			meteorCount;
	bool			crystalBell;
	bool			playingSound; //if falase no sound will be on
	
	//for making up the score
	int				flowersEaten;
	int				preditorKilled;
	
	//weather optimize
	bool		rain;
	bool		snow;
	
	float alliesRadius;
	bool haveFogofwar;
	GLuint fogofwarTex;

	float flagRot;
	float flagRot2;

	//first person movement
	float pitch;
	float yaw;
};