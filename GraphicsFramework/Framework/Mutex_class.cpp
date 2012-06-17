#include "Mutex_class.h"

Mutex_class::Mutex_class() {
	mutex = CreateMutex(
		NULL, // default security
		FALSE, // initially not owned
		NULL); // unamed mutex
}

Mutex_class::~Mutex_class() {
	CloseHandle(mutex);
}

/*
wait 1 millisecond for the mutex, if its locked then return false, else true
*/
bool Mutex_class::lock_mutex() {
	DWORD wait_result;
	wait_result = WaitForSingleObject(
		mutex, // handle to mutex
		5); // 1 millisecond wait
	return wait_result != WAIT_TIMEOUT;
}

/*
Lock mutex, else wait until unlocked to lock
*/
void Mutex_class::lock_mutex_wait(){
	DWORD wait_result;
	wait_result = WaitForSingleObject(
		mutex, // handle to mutex
		INFINITE); // no time-out interval
}

void Mutex_class::unlock_mutex() {
	ReleaseMutex(mutex);
}
