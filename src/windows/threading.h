
#ifndef DIRLUCKY_WINTHREADING_H
#define DIRLUCKY_WINTHREADING_H

#include <windows.h>

typedef HANDLE THREAD;
typedef HANDLE MUTEX;

int init_mutex(MUTEX* lock) {
	MUTEX res = CreateMutexA(NULL, false, NULL);

	if(res){
		*lock = res;
		return 0;
	}

	return -1;
}

int free_mutex(MUTEX* lock) {
	return !CloseHandle(*lock);
}

int lock_mutex(MUTEX* lock) {
	return WaitForSingleObject(*lock, INFINITE) == WAIT_FAILED;
}

int unlock_mutex(MUTEX* lock) {
	return !ReleaseMutex(*lock);
}

int create_thread(THREAD* thread, void *(* func)(void *), void* arg) {
	DWORD tid;
	HANDLE res = CreateThread(NULL, 0, func, arg, 0, &tid);

	if(res){
		*thread = res;
		return 0;
	}

	return -1;
}

int join_thread(THREAD thread) {
	int res = WaitForSingleObject(thread, INFINITE);

	return res == WAIT_FAILED;
}

int destroy_thread(THREAD thread) {
	return !CloseHandle(thread);
}

#endif //DIRLUCKY_WINTHREADING_H
