#ifndef _PARTICLEEMITTER_H_
#define _PARTICLEEMITTER_H_

#include "ParticleEmitter.h"
#include "../Framework/GameObjectTypes.h"
#include "../Framework/PhysicsThread.h"
//#include "Collision.h"

enum SpellTypes
{
	REDRED,
	REDGREEN,
	REDBLUE,
	REDYELLOW,
	REDPURPLE,
	REDCYAN,
	GREENGREEN,
	GREENRED,
	GREENBLUE,
	GREENYELLOW,
	GREENPURPLE,
	GREENCYAN,
	BLUEBLUE,
	BLUERED,
	BLUEGREEN,
	BLUEYELLOW,
	BLUEPURPLE,
	BLUECYAN,
	SPELLMAX
};

#define PARTICLENUMBER 1000
#define PARTICLELIMIT 1000 / 2
#define PARTICLESTREAM 42

class Spells :public ParticleEmitter
{
	public:
		Spells();
		Spells(Vector3 pos1, Vector3 dir1, Vector3 dir2, Vector3 dir3);
		~Spells();

		virtual void Update(float msec);
		void UpdateStream(float msec);
		void UpdateStream(float msec, bool b);
		void fireSpell(); 
		void fireRay();
		//void stopRay() { ray = false; lastStreamCount = 0; ReturnSpell((SpellTypes)(rand() % SPELLMAX)); }
		void stopRay() { ray = false; ReturnSpell((SpellTypes)(rand() % SPELLMAX)); }
		void setInitialPosition(Vector3 pos) { initialPosition = pos; } 
		Vector3 getPosition() const { return currentPosition; }
		float getRadius() const { return radius; }
		bool isActive() const { return active; }
		bool isRayActive() const { return activeRay; }
		void spellHit() { hit = true; }
		void rayHit(Vector3 cd) { rHit = true; collision = cd; }
		virtual void Draw();
		virtual void ResizeArrays();
		inline void setSpell(SpellTypes st) { ReturnSpell(st); }
		inline bool checkCollision() const { return collisionCheck; }

		void SetRigidBody(PhysicsThread* _engine);

		GameObject gameObj;


	protected:

		int lastStreamCount;
		bool fire;
		bool ray;
		bool active;
		bool activeRay;
		bool hit;
		bool rHit;
		bool explode;
		bool lockSpell;
		bool collisionCheck;
		float variance;
		float radius;
		Vector4* retSpell;
		Vector3 initialPosition;
		Vector3 currentPosition;

		void ReturnSpell(SpellTypes s);
		void leaveTrail(Particle*);
		void clearLiveParticles();

		vector<Particle*>	explosion;
		Vector3 collision;
		Vector3 pos;
		Vector3 direction[3];

	private:
		PhysicsThread* engine;
		int rigidBody;
};

#endif