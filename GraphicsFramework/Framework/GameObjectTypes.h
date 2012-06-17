#ifndef _GAMEOBJECTTYPES_H_
#define _GAMEOBJECTTYPES_H_

#include "Mutex_class.h"

enum GOTypes
{
	DEFAULT_TYPE,
	KINGPOOKA,
	POOKA,
	PREDATOR,
	CRYSTAL,
	POWERUP,
	FLOWER,
	TREE,
	MUSHROOM,
	LIGHT,
	PEDESTAL,
	FALLING,
	SPELL,
	METEOR,
	BOULDER,
	HEALTHSPHERE,
	MAX_TYPES
};

enum GOStates
{
	DEFAULT_STATE,
	ALIVE,
	DEAD,	
	INGROUP,
	OUTSIDEGROUP,
	MOVING,
	WANDERING,
	ATTACKING,
	MAX_STATES
};

class GameObject
{
	public:
		GameObject(GOTypes type = DEFAULT_TYPE, GOStates state = DEFAULT_STATE, bool anim = false) { GOType = type; GOState = state; animated = anim;currentAnimFrame=0; }
		~GameObject() {};

		inline void setType(GOTypes type) 
		{ 
			if(objMut.lock_mutex())
			{
				GOType = type; 
			}
			objMut.unlock_mutex();
		}
		inline GOTypes getType() const { return GOType; }
		inline void setState(GOStates state) 
		{ 
			if(objMut.lock_mutex())
			{
				GOState = state;
			}
			objMut.unlock_mutex();
		}
		inline int getHp() {return hp;}
		inline void setHp(int num)
		{
			if(objMut.lock_mutex())
			{
				hp = num;
			}
			objMut.unlock_mutex();
		}
		inline GOStates getState() const { return GOState; }
		inline bool getAnimated() {return animated;}
		inline void setAnimated(bool a) {animated = a;}
		inline void setAnimFrame(unsigned int frame){currentAnimFrame = frame;}
		unsigned int getAnimFrame()const { return currentAnimFrame;}
	protected:

	private:
		int hp;
		bool animated;
		GOTypes GOType;
		GOStates GOState;
		Mutex_class objMut;
		unsigned int currentAnimFrame;
};

#endif