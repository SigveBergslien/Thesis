#pragma once

#include <windows.h>

/*

*/
class Mutex_class
{
public:
	Mutex_class();
	virtual ~Mutex_class();
	virtual bool lock_mutex();
	virtual void lock_mutex_wait();
	virtual void unlock_mutex();
protected:
	HANDLE mutex;
};

