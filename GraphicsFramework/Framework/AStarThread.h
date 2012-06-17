#pragma once

#include "..\Framework\HeightMap.h"
#include "..\Framework\Thread.h"
#include "..\Framework\Mutex_class.h"

class AStarThread : public Thread {
public:
	virtual void run();
	void stop();
	void initialise(HeightMap* _map);
	int getPathLength(){return pathLength;}
	void setVector(Vector3 s, Vector3 e);
	Vector3* GetPath(){return path;}
	bool MakingPath(){
		if(mut.lock_mutex())
		{
			mut.unlock_mutex();
			return makePath;
		}
		else
			return true;
	}
private:
	bool running;
	HeightMap* map;
	Vector3* path;
	Vector3 begin;
	Vector3 end;
	Vector3 startCopy;
	Vector3 endCopy;
	int pathLength;
	bool makePath;

	Mutex_class mut;
};