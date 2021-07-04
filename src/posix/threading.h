
#ifndef DIRLUCKY_POSIXTHREADING_H
#define DIRLUCKY_POSIXTHREADING_H

#include <pthread.h>

typedef pthread_t THREAD;
typedef pthread_mutex_t MUTEX;


int init_mutex(MUTEX* lock){
	return pthread_mutex_init(lock, NULL);
}

int free_mutex(MUTEX* lock){
	return pthread_mutex_destroy(lock);
}

int lock_mutex(MUTEX* lock){
	return pthread_mutex_lock(lock);
}

int unlock_mutex(MUTEX* lock){
	return pthread_mutex_unlock(lock);
}

int create_thread(THREAD* thread, void *(* func)(void *), void* arg){
	return pthread_create(thread, NULL, func, arg);
}

int join_thread(THREAD thread){
	return pthread_join(thread, NULL);
}

int destroy_thread(THREAD* thread) {
	return 0;
}

#endif //DIRLUCKY_POSIXTHREADING_H
