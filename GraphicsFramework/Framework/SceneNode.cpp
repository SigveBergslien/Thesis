#include "SceneNode.h"

SceneNode::SceneNode(Mesh *mesh, Vector4 colour)
  {
	this->mesh = mesh;
	this->colour = colour;
	this->boundingRadius = 1.0f;
	this->distanceFromCamera = 0.0f;
	this->modelScale = Vector3(1, 1, 1);
	this->parent = NULL;
	rigidBody=-1;

	//path
	pathV = NULL;
	FPS = false;
	pathSize = 0;
	movingTowards = 0;
	needPath = false;
	velocity = Vector3(0.0f, 0.0f, 0.0f);
	lastVelocity = Vector3(1.0f,0.0f, 1.0f);

	direction = Vector3(0.0f,0.0f,0.1f); 
}

SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode *s)
{
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(SceneNode *s)
{
	for (vector<SceneNode *>::const_iterator i = s->parent->GetChildIteratorStart(); i != s->parent->GetChildIteratorEnd(); ++i)
	{
		if (*i == s)
		{
			s->parent->children.erase(i);
			break;
		}
	}
	delete s;
}

void SceneNode::updatePosition(float msec)
{
	if (pathV == NULL)
		return;
	if ((GetWorldTransform().GetPositionVector() - pathV[movingTowards]).SqrLength() > 90&&(GetWorldTransform().GetPositionVector() - pathV[movingTowards+1]).SqrLength()>256)
	{
		Vector3 vel = pathV[movingTowards] - GetWorldTransform().GetPositionVector();
		vel.Normalise();
		vel=vel*70;
		addVelocity(vel*(msec/1200), 10);
		return;
	}
	else if (++movingTowards == pathSize)
	{
		delete[] pathV;
		pathV=NULL;
		movingTowards = 0;
		return;
	}
	else
	{
		updatePosition(msec);
		return;
	}
}

void SceneNode::Update(float msec)
{
	if(engine->getBody(rigidBody))
	{
		if (!FPS)
		{
			if (engine->getBody(rigidBody)&&objData.getType()!=BOULDER)
			{
				Matrix4 rot;
				rot.ToIdentity();

				Vector3 a = -engine->getBody(rigidBody)->GetVelocity();

				if(abs(a.Length()) < 0.1)
				{
					SetTransform(transform);
				}
				else
				{
					velocity = a;

					if(abs(a.Length()) < 1.0f) a = lastVelocity;
					else lastVelocity = a;
			

					a.y = 0.0f;
					a.Normalise();
					rot.values[8] = a.x;
					rot.values[9] = a.y;
					rot.values[10] = a.z;
			
					rot.values[4] = 0.0f;
					rot.values[5] = 1.0f;
					rot.values[6] = 0.0f;

					Vector3 b = Vector3::Cross(Vector3(0.0, 1.0f, 0.0f), a);
			
					rot.values[0] = b.x;
					rot.values[1] = b.y;
					rot.values[2] = b.z;

					SetTransform(Matrix4::Translation(engine->getBody(rigidBody)->GetPosition())* rot);
				}
			
				if(objData.getState()==DEAD && objData.getType() != PREDATOR)
				{
					SetModelScale(Vector3(3.0f,0.2f,3.0f));
				}

		//		SetTransform(Matrix4::Translation(engine->getBody(rigidBody)->GetPosition()));
			}
			else
			{
				SetTransform(Matrix4::Translation(engine->getBody(rigidBody)->GetPosition())*engine->getBody(rigidBody)->GetOrientation());
				if(objData.getState()==DEAD)
				{
					SetModelScale(Vector3(0.0f,0.0f,0.0f));
				}
				else
				{
					SetModelScale(Vector3(40.0f,40.0f,40.0f));
				}
			}
		}
	}

	if (parent) // This node has a parent...
	{
		worldTransform = parent->worldTransform * transform;
	}
	else // Root node, world transform is local transform!
	{
		worldTransform = transform;
	}

	for (vector<SceneNode *>::iterator i = children.begin(); i != children.end(); ++i)
	{
		(*i)->Update(msec);
	}
	//gamePlayUpdate(msec);
	updatePosition(msec);
}

void SceneNode::SetRigidBody(float radius, PhysicsThread* _engine, float mass){
	engine = _engine;
	rigidBody = engine->addRigidBody(GetTransform().GetPositionVector(), radius,mass,&objData);
}

void SceneNode::addVelocity(Vector3 vel,int i)
{
	if (rigidBody != -1)
	{
		engine->setVelocity(rigidBody,vel);
	}
}

Vector3 SceneNode::GetTargetPosition()
{
	if(pathV)
	{
		return(pathV[movingTowards]);
	}
	else
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}
}

void SceneNode::gamePlayUpdate(float msec)
{
	//Predator
	//if(objData.getType() == PREDATOR)
	//{
	//	if(engine->getBody(rigidBody)->getColType()==5)
	//		//hp--
	//		int i =0;
	//	float disFromFlock = (engine->getBody(rigidBody)->GetPosition() - engine->getFlockPos()).SqrLength();
	//	if(disFromFlock < 2400)
	//	{
	//		// set state moving towards
	//		if(disFromFlock < 1000)
	//		{
	//			//set state chase
	//		}
	//	}
	//}
	//if(objData.getType() == PREDATOR)
	//{
	//	if(objData.getState()==ALIVE)
	//	{
	//		int ranDir = rand()%400;
	//		//New random direction
	//		if(engine->getBody(rigidBody)->getColType()==4){
	//			ranDir =1;
	//			engine->getBody(rigidBody)->SetColType(0);
	//		}
	//		if(ranDir ==1)
	//		{
	//			Vector3 tmpVelo = Vector3(rand()%3-1.0,0,rand()%3-1.0);
	//			tmpVelo.Normalise();
	//			direction = tmpVelo*50;
	//		}
	//		addVelocity(direction*(msec/1000),0);
	//	}
	//	if(objData.getState()==MOVING){
	//		Vector3 dir = engine->getFlockPos() - engine->getBody(rigidBody)->GetPosition();
	//		dir.Normalise();
	//		dir = dir*80;
	//		addVelocity(dir*(msec/1000),0);
	//	}
	//}
}

void SceneNode::clearPath()
{ 	
	if (pathV)
	{
		delete [] pathV; 
		pathV = NULL; 
		pathSize = 0; 
		movingTowards = 0; 
		needPath = false;	
	}
		
}