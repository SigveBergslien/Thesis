#pragma once
#include "../Framework/SceneNode.h"
#include "../Framework/MD5Mesh.h"
#include "../Framework/Enemy.h"
#include "../Framework/SoundSystem.h"

class StateMachine
{
public:
	StateMachine(SceneNode* n, MD5Mesh* m, PhysicsThread* e, const int rb);
	~StateMachine(){};

	void CheckState(float msec);
	void UpdateState(float msec);

	//States for different mesh's
	void KingPookaState(float msec);
	void PredatorState(float msec);

	unsigned int getAnimFrame()const { return currentAnimFrame;}

	// I'ma in your state machine, screwing your shit!

protected:
	//strings to indicate current animation
	string currentKingAnim;		//king
	string kingPookaStand;
	string kingPookaWalk;

	string currentPredatorAnim; //predator
	string predatorAttack;
	string predatorWalk;
	string predatorMove;
	string predatorDead;
	string predatorAttack2;
	//strings to indicate mesh's location 
	string kingPookaLoc;
	string predatorLoc;

	//node & mesh being brought in
	SceneNode* node;
	MD5Mesh* mesh;
	int rigidBody;
	PhysicsThread* engine;
	string currentAnimName;
	MD5Skeleton* skeleton;
	MD5Anim* currentAnim;
	unsigned int	currentAnimFrame;
	float	frameTime;	
	int		soundPlaying;

	int currentMs;
};