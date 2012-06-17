#include "StateMachine.h"


//StateMachine::StateMachine(SceneNode* n, MD5Mesh* m, PhysicsThread* e, const int rb) : node(n), mesh(m), engine(e), rigidBody(rb)
//{	
//	currentAnimFrame = rand() % 10;
//	frameTime = 0;
//	kingPookaLoc =  "../Meshes/pookaKing.md5mesh";
//	predatorLoc = "../Meshes/predator.md5mesh";	
//	kingPookaStand = "../Meshes/pookaStand.md5anim";
//	kingPookaWalk = "../Meshes/pookaWalk.md5anim";
//	predatorAttack = "../Meshes/predAttack.md5anim";
//	predatorWalk = "../Meshes/predWalk.md5anim";	
//	predatorMove = "../Meshes/predMove.md5anim";
//	predatorAttack2 = "../Meshes/predAttack2.md5anim";
//	if (node->objData.getType() == PREDATOR)
//	{
//		((Predator*)(&node))->SetEnemyHP(100);
//	}
//	skeleton = new MD5Skeleton(mesh->GetCurrentSkeleton());
//	currentAnim		 = NULL;	
//	soundPlaying	= 0;
//}

StateMachine::StateMachine(SceneNode* n, MD5Mesh* m, PhysicsThread* e, const int rb) : node(n), mesh(m), engine(e), rigidBody(rb)
{	
	currentMs = 0;
	currentAnimFrame = rand() % 10;
	frameTime = 0;
	kingPookaLoc =  "../Meshes/pookaKing.md5mesh";
	predatorLoc = "../Meshes/predator.md5mesh";	
	kingPookaStand = "../Meshes/pookaStand.md5anim";
	kingPookaWalk = "../Meshes/pookaWalk.md5anim";
	predatorAttack = "../Meshes/predAttack.md5anim";
	predatorWalk = "../Meshes/predWalk.md5anim";	
	predatorMove = "../Meshes/predMove.md5anim";
	predatorDead = "../Meshes/predatorDead.md5anim";
	predatorAttack2 = "../Meshes/predAttack2.md5anim";
	if (node->objData.getType() == PREDATOR)
	{
		((Predator*)(node))->objData.setHp(100);
	}
	skeleton = new MD5Skeleton(mesh->GetCurrentSkeleton());
	currentAnim		 = NULL;	
	soundPlaying	= 0;
}

void StateMachine::CheckState(float msec)
{
	//if king
	if (mesh->GetCurrentMesh() == kingPookaLoc)
	{		
		KingPookaState(msec);	
	}

	//if predator
	else if (mesh->GetCurrentMesh() == predatorLoc)
	{
		PredatorState(msec);
	}
}


void StateMachine::UpdateState(float msec)
{	

		CheckState(msec);	
	
	
		//float newMsec = msec/1.5;
		if(node->objData.getState() != DEAD)
			mesh->UpdateAnim(msec, skeleton, currentAnim, currentAnimFrame, frameTime);
		else if(node->objData.getState() == DEAD && currentAnimFrame < 39)
		{
			mesh->UpdateAnim(msec, skeleton, currentAnim, currentAnimFrame, frameTime);

		}
		else
		{
			mesh->UpdateAnim(0, skeleton, currentAnim, currentAnimFrame, frameTime);
		}

		//mesh.UpdateAnim(msecl, skeleton, currentAnim, currentAnimFrame, frameTime);

		node->objData.setAnimFrame(currentAnimFrame);
		
}

void StateMachine::KingPookaState(float msec)
{
	if(engine->getBody(rigidBody))
	{
		if (engine->getBody(rigidBody)->GetVelocity().SqrLength() > 100)
		{	

			//walk
			if (currentAnimName != kingPookaWalk)
			{	
				currentAnim = mesh->PlayAnim(kingPookaWalk, currentAnim, currentAnimFrame, frameTime);
				currentAnimName = kingPookaWalk;
			}
		}

		else 
		{
			//stand(idle)
			if (currentAnimName != kingPookaStand)
			{			
				currentAnim = mesh->PlayAnim(kingPookaStand, currentAnim, currentAnimFrame, frameTime);
				currentAnimName = kingPookaStand;
			}		
		}
	}
}

void StateMachine::PredatorState(float msec)
{	
	currentMs += msec;
	if(engine->getBody(rigidBody))
	{
		//if a spell hits
		if(engine->getBody(rigidBody)->getColType()==5)
		{
			//((Predator*)(node))->SetEnemyHP(((Predator*)(node))->GetEnemyHP()-10);
			int i =0;
		}
		float disFromFlock = (engine->getBody(rigidBody)->GetPosition() - engine->getFlockPos()).Length();
		int i =0;

		if(node->objData.getState() != DEAD)
		{
			//MOVING TOWARDS FLOCK
			if(disFromFlock < 350)
			{
				if (node->objData.getState() != MOVING)
					node->objData.setState(MOVING); 
				// set state moving towards
				if(disFromFlock < 60)
				// set state moving towards			
				{
					//ATTACKINGFLOCK
					if (node->objData.getState() != ATTACKING)
						node->objData.setState(ATTACKING);  
									
				}
			}
			else  if (disFromFlock > 500 && node->objData.getState() != MOVING)
			{
				if (node->objData.getState() != WANDERING)
				node->objData.setState(WANDERING);
			}
		}
		if (((Predator*)(node))->objData.getHp() <= 0)
		{
			node->objData.setState(DEAD);	
		}
	

		//if wandering state
		if(node->objData.getState() == WANDERING)
		{
			if (currentAnimName != predatorWalk)
			{
				currentAnim = mesh->PlayAnim(predatorWalk, currentAnim, currentAnimFrame, frameTime);
					currentAnimName = predatorWalk;
			}
			
			int ranDir = 0;
			if(currentMs >500)
			{
				ranDir = rand()%400;
				currentMs -= 500;
			}
			//New random direction
			//if(engine->getBody(rigidBody)->getColType() == 4)
			//{
				//ranDir =1;
			//	engine->getBody(rigidBody)->SetColType(0);
			//}

			else
				ranDir = 2;

			if(ranDir ==1)
			{

				float tmpx = rand()%3-1.0f;
				float tmpz = rand()%3-1.0f;

				Vector3 tmpVelo = Vector3(tmpx,0,tmpz);
				tmpVelo.Normalise();
				node->SetDirection(tmpVelo);
			}

			node->addVelocity(node->GetDirection()*(msec/1000),0);
		}
		else if (node->objData.getState() == ATTACKING)
		{
			
			if (currentAnimName != predatorAttack2)
				{
					currentAnim = mesh->PlayAnim(predatorAttack2, currentAnim, currentAnimFrame, frameTime);
					currentAnimName = predatorAttack2;
				}
			Vector3 dir = Vector3(0,0,0);
			if(engine->getBody(rigidBody))
				dir = engine->getFlockPos()-engine->getBody(rigidBody)->GetPosition();
			dir.Normalise();
			dir = dir*100;
			node->addVelocity(dir*(msec/1000),0);

			if(soundPlaying == 0)
			{

				if(Sound::GetSound("../Sounds/zombieattack.wav")) {			
					SoundSystem::GetSoundSystem()->PlaySound(Sound::GetSound("../Sounds/zombieattack.wav"),SOUNDPRIORITY_HIGH);
					soundPlaying = 100;
				}

			}else{
				soundPlaying--;
			}
		}

		//if moving state
		else if(node->objData.getState() == MOVING)
		{
			if (currentAnimName != predatorMove)
			{
				currentAnim = mesh->PlayAnim(predatorMove, currentAnim, currentAnimFrame, frameTime);
					currentAnimName = predatorMove;
			}
			Vector3 dir = Vector3(0,0,0);
			if(engine->getBody(rigidBody))
				dir = engine->getFlockPos()-engine->getBody(rigidBody)->GetPosition();

			dir.Normalise();
			dir = dir*100;
			node->addVelocity(dir*(msec/1000),0);
		}

		else if (node->objData.getState() == DEAD)
		{
			if (currentAnimName != predatorDead)
			{
				currentAnim = mesh->PlayAnim(predatorDead, currentAnim, currentAnimFrame, frameTime);
					currentAnimName = predatorDead;
					currentAnimFrame = 4;
			}
		}
		}
}