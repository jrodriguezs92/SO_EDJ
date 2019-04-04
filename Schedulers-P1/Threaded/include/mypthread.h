/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 04/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <ucontext.h>
#include <stdbool.h>
#include <signal.h>
#include <structs.h>

#define SRR 0
#define LOTTERY 1
#define RT 2
#define RR 3


// Threads prototypes
int pthread_create(pthread_t*, void*, void *(*start_routine) (void *), void *arg); // mypthread_create
void pthread_exit(void*); // mypthread_end
int pthread_join(pthread_t id, void **result); // mypthread_join
int pthread_yield(void); // mypthread_yield
int pthread_detach(pthread_t thread); // mypthread_detach
int pthread_mutex_init(pthread_mutex_t* restrict mutex, const pthread_mutexattr_t* restrict attr); // mymutex_init
int pthread_mutex_destroy(pthread_mutex_t* mutex);// mymutex_destroy
int pthread_mutex_lock(pthread_mutex_t* mutex);// mymutex_lock
int pthread_mutex_unlock(pthread_mutex_t* mutex);// mymutex_unlock
int pthread_mutex_trylock(pthread_mutex_t* mutex);// mymutex_trylock
// mythread_setsched