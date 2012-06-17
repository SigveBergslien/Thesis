#include "AStarThread.h"

void AStarThread::initialise(HeightMap* _map){
	map = _map;
	path = NULL;
	begin = Vector3(0,0,0);
	end = Vector3(0,0,0);
	startCopy = Vector3(0,0,0);
	endCopy = Vector3(0,0,0);
	pathLength = 0;
	makePath=false;
	path=NULL;
}

void AStarThread::run(){
	while(running){
		if(makePath){
			if(mut.lock_mutex()){
				begin = startCopy;
				end = endCopy;
			}
			mut.unlock_mutex();
			path = map->pathfinding(begin,end,pathLength);
			if(mut.lock_mutex()){
				makePath = false;
			}
			mut.unlock_mutex();
		}
	}
}

void AStarThread::stop(){
	running = false;
}

void AStarThread::setVector(Vector3 s, Vector3 e){
	if(mut.lock_mutex()){
		startCopy =s;
		endCopy = e;
		makePath = true;
	}
	mut.unlock_mutex();
}