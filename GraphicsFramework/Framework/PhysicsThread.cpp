#include "PhysicsThread.h"

/*
Deletes teh physics thread with all bodies and flock.
*/
PhysicsThread::~PhysicsThread()
{
	for(int i =0; i<NUM_BODIES;i++)
	{
		if(bodies[i])
			delete bodies[i];
		if(tmpBodies[i])
		{
			delete tmpBodies[i];
		}
	}
	delete bodies;
	delete tmpBodies;
	delete flock;
	delete flockBodies;
	//delete rootOctTree;
}
/*
Initialise the physics thread, this needs to be done before starting
The window is sent as parameter for the physics thread to have access 
to the timer.
*/
void PhysicsThread::initialize(Window &parent)
{
	bodies = new RigidBody*[NUM_BODIES];
	tmpBodies = new RigidBody*[NUM_BODIES];
	gameObj = new GameObject*[NUM_BODIES];

	//Octree Stuff
	//rootOctTree = new Octree();
	//rootOctTree->SetSceneDimensions(Vector3(0,0,0),Vector3(RAW_HEIGHT*HEIGHTMAP_X ,RAW_HEIGHT*HEIGHTMAP_X, RAW_HEIGHT*HEIGHTMAP_X));
	//countTestOctree = 0; 
	//octreeResetTimer = 0;
	//Sets bodies to NULL
	for(int i =0;i<NUM_BODIES;i++)
	{
		bodies[i] = NULL;
		tmpBodies[i]=NULL;
		tmpVelo[i] = Vector3(0,0,0);
	}
	w = parent.GetWindow();
	flock = NULL;
	numberOfBodies = 0;
	addFlock = false;
	clothUpdateing = false;
	kingPos = Vector3(0,0,0);
	flockBodies= NULL;
	pause = false;
	numberInGroup =0;
	flockPos = Vector3 (0,0,0);

	flock = new Flock();
	flockBodies = new RigidBody*[NUM_BODIES];
	reset = false;
	meteorDirection = Vector3(0,0,0);
}

void PhysicsThread::setResetPhysics()
{
}
void PhysicsThread::resetPhysics()
{
	for(int i =0; i<NUM_BODIES;i++)
	{
		if(bodies[i])
			delete bodies[i];
		if(tmpBodies[i])
		{
			delete tmpBodies[i];
		}
	}

	for(int i =0;i<NUM_BODIES;i++)
	{
		bodies[i] = NULL;
		tmpBodies[i]=NULL;
		tmpVelo[i] = Vector3(0,0,0);
	}
	flock->resetFlock();
	numberOfBodies = 0;
	addFlock = false;
	clothUpdateing = false;
	map=NULL;
	cape=NULL;
	kingPos = Vector3(0,0,0);
	flockBodies= NULL;
	pause = false;
	numberInGroup =0;
	flockPos = Vector3 (0,0,0);
	reset = false;
}

/*
Run function for the physics thread. A seperate timer is used and the thread will sleep
if it is running too fast.
*/
void PhysicsThread::run()
{
	while(running)
	{		
		float timer = (w->GetPhysicsTimer()->GetTimedMS());
		
		if(timer>200)
			timer = 20;
		if(pause)
			Sleep(18);
		else
		{
			if(timer<15)
			{
				Sleep(8);
				timer+=11;
			}
			collisions();
			Integrate(timer);

			if(mut.lock_mutex())
			{
				if(kingPos != Vector3(0,0,0))
				{
					bodies[0]->SetPosition(kingPos);
					kingPos = Vector3(0,0,0);
					bodies[0]->resetVelocity();
				}
				int end =0;
				for(int i=0; i<NUM_BODIES;i++)
				{
					if(bodies[i]){
						bodies[i]->AddVelocity(tmpVelo[i]);
						tmpVelo[i] = Vector3(0,0,0);
					}
					else
					{
						end =i;
						break;
					}
				}
				for(int i=end; i<numberOfBodies;i++)
				{
					bodies[i] = tmpBodies[i];
					tmpBodies[i] = NULL;
				}
			}
			mut.unlock_mutex();

			if(flockMut.lock_mutex())
			{
				if(addFlock)
				{
					int counter=0;
					int size = 0;
					for(int i =0;i<NUM_BODIES;i++)
					{
						if(counter == NUM_BODIES)
							break;
						flockBodies[i] = NULL;
						while(bodies[counter])
						{
							counter++;
							if((gameObj[counter-1]->getType() == POOKA&&gameObj[counter-1]->getState() == INGROUP)||gameObj[counter-1]->getType() == KINGPOOKA)
							{
								flockBodies[i] = bodies[counter-1];
								size = i+1;
								break;
							}
						}
					}
					numberInGroup = size;
					flock->initializeFlock(flockBodies, size);
					addFlock = false;
				}

				if(flock)
				{
					flock->updateFlock(timer);
					flockPos = flock->GetAvgPos();
				}
			}
			flockMut.unlock_mutex();
			//Either call octree for the 1st time or recreate()
			/*if(countTestOctree > 0){
				rootOctTree->DrawOctree(rootOctTree,frustum);
				octreeResetTimer++;
			}
			if(numberOfBodies > 50 && countTestOctree < 1){
				countTestOctree++;
				rootOctTree->CreateNode(bodies, numberOfBodies, rootOctTree->GetCenter(), rootOctTree->GetDimension()); 
			}
			if(octreeResetTimer > 10){
				RecreateOctree();
			}*/
		}
	}
}
/*
Stop the thread, so that it can be deleted.
*/
void PhysicsThread::stop()
{
	running = false;
}
/*
Add a rigid Body to the thread as well as the gameObject, which holds the type and state of the rigid Body
The rigid Body will be added to a temporary array and added in the main loop of the thread.
*/
int PhysicsThread::addRigidBody(Vector3 pos, float radius, float mass, GameObject* obj)
{
	
	mut.lock_mutex();
	tmpBodies[numberOfBodies] = new RigidBody(pos, mass);
	tmpBodies[numberOfBodies]->SetTypeSphere((int)radius);
	gameObj[numberOfBodies] = obj;
	numberOfBodies= numberOfBodies+1;
	mut.unlock_mutex();
	return numberOfBodies -1;
}

/*
Integrate The rigid bodies added to the engine.
*/
void PhysicsThread::Integrate(float msec)
{
	for(int i =0;i<NUM_BODIES;i++)
	{
		if(bodies[i])
		{
			if(gameObj[i]->getType()==BOULDER&&gameObj[i]->getState()==ALIVE)
			{
				if(meteorDirection==Vector3(0.0,0.0,0.0))
				{
					bodies[i]->setMeteor();
					Vector3 tmp = bodies[0]->GetPosition()-bodies[i]->GetPosition();
					tmp.Normalise();
					meteorDirection = tmp;
					bodies[i]->AddForce(meteorDirection*800000000);
				}
				if(bodies[i]->GetPosition().y>40)
					//bodies[i]->AddForce(meteorDirection*50000);
					bodies[i]->AddVelocity(Vector3(0,-5.0f*(msec/80.0f),0));
			}
			if(gameObj[i]->getType()==FALLING)
				bodies[i]->AddVelocity(Vector3(0,-9.8f*(msec/80.0f),0));
			if(gameObj[i]->getState() !=DEAD)
				bodies[i]->Integrate(msec/1000);
		}
		else
			break;
	}
}
/*
Collision Detection between objects, as well as the heightmap.
Change state of objects using gameObj when collision occour
*/
void PhysicsThread::collisions()
{
	for(int i =0; i < NUM_BODIES;i++)
	{
		if(bodies[i])
		{
			if(gameObj[i]->getType()==FALLING&&bodies[i]->GetPosition().y<0.1f)
			{
				//bodies[i]->SetType(4);
				gameObj[i]->setState(DEAD);
			}
			if(gameObj[i]->getState()!=DEAD&&gameObj[i]->getType()!=FALLING)
			{
				if(map)
				{
					if(!clothUpdateing&&cape&&i==0){
						colDet.Collision(*bodies[i],cape);
					}
					//rock
					if(gameObj[i]->getType()==BOULDER)
					{
						int numTriangles = 0;
						const BoundingTriangle* tmpTriangle= map->returnTriangleList(Vector2(bodies[i]->GetPosition().x,bodies[i]->GetPosition().z),numTriangles);
						CollisionData* colDatm = new CollisionData();
						for(int j=0;j<numTriangles;j++)
						{
							if(colDet.Collision(*bodies[i],tmpTriangle[j],colDatm))
							{
								colRes.CollideI(colDatm,*bodies[i]);
						}
					}
					delete colDatm;
				}
					if(gameObj[i]->getType()!=SPELL)
					{
						Vector3* tmpVert = map->surrondingVerts(bodies[i]->GetPosition());
						if(tmpVert)
						{
							Vector3 tmpVel = Vector3(tmpVert->x,0,tmpVert->z) - bodies[i]->GetPosition();
							tmpVel.Normalise();
							bodies[i]->AddForce(-tmpVel*250);
							if(gameObj[i]->getType() ==PREDATOR)
							{
								bodies[i]->SetColType(4);
							}
						}
					}
				}
				for(int j = i+1; j< NUM_BODIES; j++)
				{
					if(bodies[j])
					{
						
						if(gameObj[j]->getState()!=DEAD&&gameObj[j]->getType()!=FALLING)
						{
							CollisionData* colDat = new CollisionData();
							if(colDet.Collision(*bodies[i],*bodies[j],colDat))
							{
								//if rock
								if(gameObj[j]->getType()==BOULDER &&gameObj[i]->getType()!=BOULDER )
								{
									if(gameObj[i]->getType()== SPELL)
									{
										gameObj[i]->setState(DEAD);
										gameObj[i]->setState(DEAD);
									}
									else
									{
										if(bodies[j]->GetVelocity().SqrLength() > 100)
										{
											gameObj[i]->setState(DEAD);
											addFlock=true;
										}
										colRes.Collide(colDat,*bodies[j]);
									}
								}
								else if(gameObj[i]->getType()==BOULDER &&gameObj[j]->getType()!=BOULDER )
								{
									if(gameObj[j]->getType()== SPELL)
									{
										gameObj[i]->setState(DEAD);
										gameObj[j]->setState(DEAD);
									}
									else
									{
										if(bodies[i]->GetVelocity().SqrLength() > 100)
										{
											gameObj[j]->setState(DEAD);
											addFlock=true;
										}
										colDat->normal = colDat->normal*(-1);
										colRes.Collide(colDat,*bodies[i]);
									}
								}
								else if(gameObj[i]->getType()==TREE|| gameObj[i]->getType()==MUSHROOM)
								{
										colRes.Collide(colDat,*bodies[j]);
								}
								else if(gameObj[j]->getType()==TREE|| gameObj[j]->getType()==MUSHROOM)
								{
									colDat->normal = colDat->normal*(-1);
									colRes.Collide(colDat,*bodies[i]);
								}
								//spells
								
								else if(gameObj[j]->getType()==SPELL&&gameObj[i]->getType()==PREDATOR)
								{
									//colDat->penetration = colDat->penetration * 2;
									//colRes.Collide(colDat,*bodies[i],*bodies[j]);
									if(gameObj[i]->getHp()<30)
										gameObj[i]->setState(DEAD);
									else
									{
										gameObj[i]->setHp(gameObj[i]->getHp()-30);
										gameObj[i]->setState(MOVING);
									}
									gameObj[j]->setState(DEAD);
									
								}
								else if(gameObj[i]->getType()==SPELL&&gameObj[j]->getType()==PREDATOR)
								{
									//colDat->penetration = colDat->penetration * 2;
									//colRes.Collide(colDat,*bodies[i],*bodies[j]);
									if(gameObj[j]->getHp()<30)
										gameObj[j]->setState(DEAD);
									else
									{
										gameObj[j]->setHp(gameObj[i]->getHp()-30);
										gameObj[j]->setState(MOVING);
									}
									gameObj[i]->setState(DEAD);
								}
								else if(i)
								{
									colRes.Collide(colDat,*bodies[i],*bodies[j]);
									testType(i,j);
								}

								//if king
								else
								{
									colRes.Collide(colDat,*bodies[j]);
									if(gameObj[j]->getType()==POOKA&&gameObj[j]->getState()==ALIVE)
									{
										gameObj[j]->setState(INGROUP);
										addFlock = true;
									}
								}
							}
							delete colDat;
						}
					}
					else
						break;
				}
			}
		}
	}
}
/*
Test type of collison
*/
void PhysicsThread::testType(int i, int j )
{
	//if enemy
	if(gameObj[i]->getType()==PREDATOR&&gameObj[j]->getType()==POOKA)
	{
		if(gameObj[i]->getAnimFrame()==20)
		{
			if(gameObj[j]->getHp()<=25)
			{
				gameObj[j]->setType(FALLING);
				bodies[j]->AddVelocity(Vector3(0,350,0));
				addFlock=true;
			}
			else
			{
				gameObj[j]->setHp(gameObj[j]->getHp()-25);
			}
		}
	}
	else if(gameObj[i]->getType()==POOKA&&gameObj[j]->getType()==PREDATOR)
	{
		if(gameObj[j]->getAnimFrame() == 20)
		{
			if(gameObj[i]->getHp()<=25)
			{
				gameObj[i]->setType(FALLING);
				bodies[i]->AddVelocity(Vector3(0,350,0));
				addFlock=true;
			}
			else
			{
				gameObj[i]->setHp(gameObj[i]->getHp()-25);
			}
		}
	}
	//Add to flock
	if(gameObj[i]->getType()==POOKA)
	{
		if(gameObj[j]->getType()==POOKA)
		{
			if(gameObj[i]->getState()==INGROUP)
			{
				if(gameObj[j]->getState()==ALIVE)
				{
					gameObj[j]->setState(INGROUP);
					addFlock = true;
				}
			}
			else if(gameObj[i]->getState()==ALIVE)
			{
				if(gameObj[j]->getState()==INGROUP)
				{
					gameObj[i]->setState(INGROUP);
					addFlock = true;
				}
			}
		}
	}
}
/*
Add velocity to rigid body i, The velocity is stored in a temporary array which will be added
to the object in the main loop of the thread
*/
void PhysicsThread::setVelocity(int i,Vector3 vel)
{
	if(mut.lock_mutex())
	{
		tmpVelo[i] += vel;
	}
	mut.unlock_mutex();
}
/*
Return the rigid body i, Only reading can be done as it 
is not mutex locked
*/
RigidBody* PhysicsThread::getBody(int i )
{
	if(i == -1)
		return NULL;
	return bodies[i];
}
/*
The thread will add the current bodies that are in the flock to the 
flock class in the main loop.
*/
void PhysicsThread::addToFlock()
{
	addFlock = true;
}
/*
Call update on the cloth.
*/
void PhysicsThread::updateCloth(Cloth* cloth, float msec)
{
	cloth->update(msec);
}

void PhysicsThread::RecreateOctree()
{
	delete rootOctTree;
	rootOctTree = new Octree();
    rootOctTree->SetSceneDimensions(Vector3(0,0,0),Vector3(RAW_HEIGHT*HEIGHTMAP_X ,RAW_HEIGHT*HEIGHTMAP_X, RAW_HEIGHT*HEIGHTMAP_X));  
	countTestOctree = 0;
	octreeResetTimer = 0;
}
/*
Set the postion of the flock to an absolute positon, reset can be used to
return the flock to follow the first object in the bodies array.
*/
void PhysicsThread::setFlockPos(Vector3 &pos)
{
	flockMut.lock_mutex_wait();
	
	flock->setFlockPos(pos);
	flockMut.unlock_mutex();
}