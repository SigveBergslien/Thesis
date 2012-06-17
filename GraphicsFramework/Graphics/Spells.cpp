#include "Spells.h"
#include <time.h>

#define RAND() ((rand()%101)/100.0f)

Spells::Spells()
{
	particleRate		= 1.0f;
	particleLifetime	= 0.0f;
	particleSize		= 0.0f;
	particleVariance	= 0.0f;
	nextParticleTime	= 0.0f;
	particleSpeed		= 0.0f;
	numLaunchParticles	= 0;
	largestSize			= 0;
	variance = 0.0f;
	particles.clear();
	fire = false;
	ray = false;
	active = false;
	activeRay = false;
	hit = false;
	explode = false;
	lockSpell = false;
	rHit = false;
	collisionCheck = true;
	lastStreamCount = 0;

	/*
	Each particle is a white dot, which has an alpha fade on it,
	so the edges fade to 0.0 alpha.
	*/
	texture = SOIL_load_OGL_texture("../Textures/particle.tga",
	SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);
	if(!texture) exit(0);

	//texture = SOIL_load_OGL_texture("../Textures/snowflake.png",
	//	SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);
	//srand((unsigned long int)time(NULL));
	retSpell = new Vector4[2];
	ReturnSpell(GREENPURPLE);

	gameObj.setType(SPELL);
}

Spells::Spells(Vector3 pos1, Vector3 dir1, Vector3 dir2, Vector3 dir3)
{
	particleRate		= 1.0f;
	particleLifetime	= 0.0f;
	particleSize		= 0.0f;
	particleVariance	= 0.0f;
	nextParticleTime	= 0.0f;
	particleSpeed		= 0.0f;
	numLaunchParticles	= 0;
	largestSize			= 0;
	variance = 0.0f;
	particles.clear();
	fire = false;
	ray = false;
	active = false;
	activeRay = false;
	hit = false;
	explode = false;
	lockSpell = false;
	rHit = false;
	collisionCheck = true;
	lastStreamCount = 0;

	pos = pos1;
	direction[0] = dir1;
	direction[1] = dir2;
	direction[2] = dir3;

	/*
	Each particle is a white dot, which has an alpha fade on it,
	so the edges fade to 0.0 alpha.
	*/
	texture = SOIL_load_OGL_texture("../Textures/particle.tga",
	SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);
	if(!texture) exit(0);

	//texture = SOIL_load_OGL_texture("../Textures/snowflake.png",
	//	SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_COMPRESS_TO_DXT);
	//srand((unsigned long int)time(NULL));
	retSpell = new Vector4[2];
	ReturnSpell(BLUEPURPLE);
}

Spells::~Spells(void)
{
}

void Spells::Update(float msec)
{
	bool explosionset = false;
	//Enough time might have passed that more than 1 'launch' of particles is necessary...

	//while(nextParticleTime <= 0)
	//{
	//	nextParticleTime += particleRate;
		
		//Add a number of particles to the vector, obtained from the free list.
		
	if(fire && !lockSpell && !activeRay)
	{
		variance = particleSize * 2;
		clearLiveParticles();

		for(int i = 0; i < PARTICLENUMBER; ++i)
		{
			particles.push_back(GetFreeParticle());
		}

		currentPosition = initialPosition;
		radius = variance / 2.0f;
		active = true;
		explode = true;
		lockSpell = true;
	}

	//if(hit && explode && active)
	//{
	//	for(int i = 0; i < PARTICLENUMBER * 5; ++i)
	//	{
	//		particles.push_back(GetFreeParticle());
	//		particles.back()->position = currentPosition;
	//		particles.back()->velocity = particleSpeed;
	//		particles.back()->trail = false;
	//		if(i % 10)particles.back()->colour = retSpell[0];
	//		else particles.back()->colour = retSpell[1];
	//	}
	//}

	if(active)
	{
		if(hit && explode)
		{
			for(int i = 0; i < PARTICLENUMBER * 5; ++i)
			{
				particles.push_back(GetFreeParticle());
				particles.back()->position = currentPosition;
				particles.back()->velocity = particleSpeed;
				particles.back()->trail = false;
			
				if(i % 10)particles.back()->colour = retSpell[0];
				else particles.back()->colour = retSpell[1];
			}
			collisionCheck = false;
		}

		for(vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */)
		{
			Particle *p = (*i);
	
			//If this particle has ran out of life, remove it from the 'active' list,
			//and put it on the 'free' list for reuse later.

			// Initial position when fired

			if(fire)
			{
				float x, y, z;
				bool inside = false;
			
				do
				{
					x = (rand() % (int)variance) - (variance / 2);
					y = (rand() % (int)variance) - (variance / 2);
					z = (rand() % (int)variance) - (variance / 2);
			
					if((pow(x, 2) + pow(y, 2) + pow(z, 2)) <= pow((variance / 2), 2)) (inside = true);

				} while(!inside); 

				p->degrade = false;
				p->direction = initialDirection;
				p->position = initialPosition;
				p->position += (Vector3(x, y, z));
				p->velocity = particleSpeed;
				p->trail = false;

				//if(rand() % 10 != 0)p->colour = Vector4(0.0275f, 0.0275f, 0.5f, 1.0f);
				//else p->colour = Vector4(0.5f, 0.0275f, 0.0275f, 1.0f);

				if(rand() % 10 > 4)p->colour = retSpell[0];
				else p->colour = retSpell[1];
			}

			if(hit && explode)
			{
				particleLifetime = 250.0f;
				if(!p->trail)
				{
					float x, y, z;
					x = (float)(rand() % 360) - 180;
					y = (float)(rand() % 360) - 180;
					z = (float)(rand() % 360) - 180;

					p->direction = Vector3(x, y, z);
					p->direction.y += -0.1f;
					p->direction.Normalise();
					p->degrade = true;
					//p->velocity /= 100;
					//p->velocity += (rand() % 101 - 50) / 100.0f;
					p->velocity = 0.1f;
					//cout << "p velocity: " << p->velocity << endl;
					explosionset = true;
				}
				else
				{
					p->colour.w = (msec / particleLifetime) * 10.0f;
				}
			}

			if(!(rand() % 25)) leaveTrail(p);		
		
			if(p->degrade)	p->colour.w -= (msec / particleLifetime);

			if(p->colour.w <= 0.0f)
			{
				freeList.push_back(p);
				i = particles.erase(i);
				if(particles.empty()) { active = false; lockSpell = false; collisionCheck = true; }
			}
			else
			{
				//Otherwise, this particle must be still 'alive'. Update its
				//position by multiplying its normalised direction by the
				//particle speed, and adding the result to the position. Easy!
				p->direction.Normalise();
				//p->position += p->direction*(msec*particleSpeed);
				p->position += p->direction*(msec*p->velocity);

				++i;	//Only update our iterator value here.
					//If we didn't do this, we'd skip over particles when removing
					//particles from the list...
			}
		}

		fire = false;
		initialDirection.Normalise();
		currentPosition += (initialDirection * msec * particleSpeed);
		if(engine->getBody(rigidBody))
			engine->getBody(rigidBody)->SetPosition(Vector3(currentPosition.x,10,currentPosition.z));
	}
	//If we now have more particles than we have graphics memory for, we
	//must allocate some new space for them, using ResizeArrays.
	if(hit) hit = false;

	if(explode && explosionset) explode = false;

	if((particles.size()+explosion.size()) > largestSize)
	{
		ResizeArrays();	
	}
}

void Spells::UpdateStream(float msec)
{
	//Add a number of particles to the vector, obtained from the free list.
		
	if(ray && !lockSpell)
	{ 	
		variance = particleSize * 2;

		for(int i = 0; i < PARTICLESTREAM; ++i)
		{
			particles.push_back(GetFreeParticle());
			particles.back()->degrade = true;
			++lastStreamCount;
		}

		activeRay = true;
		currentPosition = initialPosition;
		radius = variance / 2.0f;
		lockSpell = true;
	}

	//Now for the particle 'think' function. Particles are so 'lightweight' there's not
	//much point putting this as a member variable of the Particle struct...

	if(activeRay)
	{
		int counter = 0;
		Vector3 furthestParticle = Vector3(0.0f, 0.0f, 0.0f);

		for(vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */)
		{
			Particle *p = (*i);
		//	if(i == particles.begin()) cout << "first position: " << p->position << endl;
			//If this particle has ran out of life, remove it from the 'active' list,
			//and put it on the 'free' list for reuse later.

			// Initial position when fired


			if(ray && (counter >= (lastStreamCount - PARTICLESTREAM)))
			{
				float x, y, z;
				bool inside = false;
			
				do
				{
					x = (rand() % (int)variance) - (variance / 2);
					y = (rand() % (int)variance) - (variance / 2);
					z = (rand() % (int)variance) - (variance / 2);
			
					if((pow(x, 2) + pow(y, 2) + pow(z, 2)) <= pow((variance / 2), 2)) (inside = true);

				} while(!inside); 

				p->degrade = true;
				p->direction = initialDirection;
				p->position = initialPosition;
				p->position += (Vector3(x, y, z));
				p->velocity = particleSpeed;

				//if(rand() % 10 != 0)p->colour = Vector4(0.0275f, 0.0275f, 0.5f, 1.0f);
				//else p->colour = Vector4(0.5f, 0.0275f, 0.0275f, 1.0f);

				if(rand() % 10 != 0)p->colour = retSpell[0];
				else p->colour = retSpell[1];
			}

		//	cout << "msec/particlelifetime: " << msec / particleLifetime << " msec: " << msec << " particleLifetime: " << particleLifetime << endl;
			if(p->degrade)	p->colour.w -= (msec / particleLifetime);

			Vector3 coltmp = Vector3(10000, 10000, 10000);

			if(rHit) { coltmp = collision; }

		//	cout << "1: " << (initialPosition - p->position).Length() << " 2: " << (initialPosition - coltmp).Length() << endl;
			if(p->colour.w <= 0.0f || ((initialPosition - p->position).Length() > (initialPosition - coltmp).Length()))
			{
				freeList.push_back(p);
				i = particles.erase(i);
				--lastStreamCount;
				if(particles.empty()) { activeRay = false; lastStreamCount = 0; }
			}
			else
			{
				p->direction.Normalise();
				p->position += p->direction*(msec*p->velocity);
			//	if((p->position - initialPosition).Length() > (furthestParticle).Length())	currentPosition = p->position;
				++i;
			}
			++counter;
		}
		if(!particles.empty()) { currentPosition = particles.front()->position; /*cout << currentPosition << endl;*/ }
		if(!explosion.empty()) activeRay = true;
		
		if(rHit || activeRay)
		{
			if(ray)
			{
				for(int i = 0; i < PARTICLESTREAM; ++i)
				{
					explosion.push_back(GetFreeParticle());
					explosion.back()->position = currentPosition;
					explosion.back()->velocity = particleSpeed;
					explosion.back()->trail = false;
					explosion.back()->degrade = true;
			
					if(i % 10)explosion.back()->colour = retSpell[0];
					else explosion.back()->colour = retSpell[1];
				}
			}

			for(vector<Particle *>::iterator i = explosion.begin(); i != explosion.end();)
			{
				Particle *p = (*i);

				float x, y, z;
				x = (float)(rand() % 360) - 180;
				y = (float)(rand() % 360) - 180;
				z = (float)(rand() % 360) - 180;

				p->direction = Vector3(x, y, z);
				p->direction.Normalise();
				p->velocity /= 2;

				if(p->degrade) p->colour.w -= (msec / particleLifetime); 

				if(p->colour.w <= 0.0f)
				{
					freeList.push_back(p);
					i = explosion.erase(i);
					if(explosion.empty()) activeRay = false; 
				}
				else
				{
					p->direction.Normalise();
					p->position += p->direction*(msec*p->velocity);
					++i;
				}
			}
		}

		ray = false;
		rHit = false;
		lockSpell = false;
		initialDirection.Normalise();
		//currentPosition += (initialDirection * msec * particleSpeed);
	}
	//If we now have more particles than we have graphics memory for, we
	//must allocate some new space for them, using ResizeArrays.
	//if(hit) hit = false;

	if((particles.size()+explosion.size()) > largestSize)
	{
		ResizeArrays();	
	}
}

void Spells::UpdateStream(float msec, bool b)
{
	int num;
	int arrpos = 0;

	(b) ? num = 3 : num = 2;

	variance = particleSize * 2;

	for(int i = 0; i < PARTICLESTREAM; ++i)
	{
		particles.push_back(GetFreeParticle());
		particles.back()->degrade = true;
		++lastStreamCount;
	}

	radius = variance / 2.0f;

	int counter = 0;


	for(vector<Particle *>::iterator i = particles.begin(); i != particles.end();/*No I++ here!!! */)
	{
		Particle *p = (*i);

		if((counter >= (lastStreamCount - PARTICLESTREAM)))
		{
			float x, y, z;
			bool inside = false;
			
			do
			{
				x = (rand() % (int)variance) - (variance / 2);
				y = (rand() % (int)variance) - (variance / 2);
				z = (rand() % (int)variance) - (variance / 2);
			
				if((pow(x, 2) + pow(y, 2) + pow(z, 2)) <= pow((variance / 2), 2)) (inside = true);

			} while(!inside); 

			p->degrade = true;
			p->direction = direction[arrpos++ % num];
			p->position = pos;
			p->position += (Vector3(x, y, z));

			if(b && (arrpos % num == 0)) p->velocity = particleSpeed * 1.6f;
			else p->velocity = particleSpeed;

			if(rand() % 10 != 0)p->colour = retSpell[0];
			else p->colour = retSpell[1];
		}

		if(p->degrade)	p->colour.w -= (msec / particleLifetime);

		if(p->colour.w <= 0.0f)
		{
			freeList.push_back(p);
			i = particles.erase(i);
			--lastStreamCount;
			if(particles.empty()) { lastStreamCount = 0; }
		}
		else
		{
			p->direction.Normalise();
			p->position += p->direction*(msec*p->velocity);
			++i;
		}
		++counter;
	}

	initialDirection.Normalise();

	if((particles.size()+explosion.size()) > largestSize)
	{
		ResizeArrays();	
	}
}

void Spells::ReturnSpell(SpellTypes s)
{
	switch(s)
	{
		case REDRED:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 5000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 5000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case REDGREEN:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case REDBLUE:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case REDYELLOW:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case REDPURPLE:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case REDCYAN:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case GREENGREEN:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case GREENRED:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case GREENBLUE:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case GREENYELLOW:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case GREENPURPLE:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case GREENCYAN:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case BLUEBLUE:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case BLUERED:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case BLUEGREEN:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case BLUEYELLOW:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}
		case BLUEPURPLE:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		case BLUECYAN:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 1000.0f, 1.0f);
			break;
		}
		default:
		{
			retSpell[0] = Vector4(PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			retSpell[1] = Vector4(PARTICLENUMBER / 10000.0f, PARTICLENUMBER / 1000.0f, PARTICLENUMBER / 10000.0f, 1.0f);
			break;
		}		
	}
}

void Spells::leaveTrail(Particle* ptl)
{
//	ptl->direction = Vector3(0, 0, 0);

	ptl->velocity /= 1.3f; 
	//ptl->velocity = 0;
	ptl->degrade = true;
	ptl->trail = true;
	return;
}

void Spells::clearLiveParticles()
{
	if(!particles.empty())
	{
		for(vector<Particle *>::iterator i = particles.begin(); i != particles.end();)
		{
			Particle *p = (*i);
			freeList.push_back(p);
			i = particles.erase(i);
		}
	}
}

void Spells::fireSpell()
{
	if(!lockSpell)
	{
		clearLiveParticles();
		fire = true;
	}
}

void Spells::fireRay()
{
	if(!lockSpell)
	{
		ray = true;
	}
}

void Spells::Draw()	{
	//Get 2 contiguous sections of memory full of our particle info
	unsigned int count = 0;

	for(unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i]  = particles.at(i)->colour;
		count++;
	}

	for(unsigned int i = count; i < (explosion.size() + count); ++i)
	{
		vertices[i] = explosion.at(i - count)->position;
		colours[i] = explosion.at(i - count)->colour;
	}

	glBindVertexArray(arrayObject);

	//Bind our vertex data, and update its data with that of the vertices array
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (particles.size()+explosion.size())*sizeof(Vector3), (void*)vertices);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);	//Tell the VAO we have positions...
	glEnableVertexAttribArray(VERTEX_BUFFER);	//Binds this buffer to the VAO
	
	//And now do the same for colours...
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (particles.size()+explosion.size())*sizeof(Vector4), (void*)colours);
	glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4), 0);
	glEnableVertexAttribArray(COLOUR_BUFFER);
	
	//We're going to use a type of alpha blending known as additive blending here. Overlapping 
	//particles will have their colours added together on screen, giving us a cool effect when
	//particles are near each other. Fire a lot of particles at once with slow speed to get a
	//'plasma ball' type effect!
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	//And now do our usual Drawing stuff...
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_POINTS,  0, particles.size()+explosion.size());	// draw ordered list of vertices
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	glBindVertexArray(0); //Remember to turn off our VAO ;)
}

void Spells::ResizeArrays()
{
	//Delete the system memory, as it is of the wrong size now...
	//yes, this does mean we're keeping particle info in THREE places...
	delete[] vertices;
	delete[] colours;

	//Tell OpenGL that we can delete our old VBOs
	glDeleteBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glDeleteBuffers(1, &bufferObject[COLOUR_BUFFER]);




	//Make some new system memory
	vertices = new Vector3[particles.size()+explosion.size()];
	colours  = new Vector4[particles.size()+explosion.size()];


	//Make some new VBO space in memory. Note how we don't actually upload any
	//data to the graphics card here! We just 'new' the memory for now.
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, (particles.size()+explosion.size())*sizeof(Vector3), 0, GL_DYNAMIC_DRAW);


	glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, (particles.size()+explosion.size())*sizeof(Vector4), 0, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	//Done with our buffers...

	largestSize = particles.size()+explosion.size();		//Update our largest particles size
}

void Spells::SetRigidBody(PhysicsThread* _engine){
	engine = _engine;
	rigidBody = engine->addRigidBody(getPosition(), 20,0,&gameObj);
}