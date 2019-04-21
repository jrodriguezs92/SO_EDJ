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
#include <time.h>

#define MYPTHREAD
#define SRR 0
#define LOTTERY 1
#define RT 2
#define RR 3

// For Selfish Round Robin scheduling
#define PA 2
#define PB 1
#define MAX_PRIORITY 1000
#define STACK_SIZE 16384

// Threads prototypes
int pthread_create(pthread_t*, void*, void *(*start_routine) (void *), void *arg); // mypthread_create
void pthread_exit(void*); // mypthread_end
int pthread_join(pthread_t id, void **result); // mypthread_join
int pthread_yield(void); // mypthread_yield
int pthread_detach(pthread_t thread); // mypthread_detach
void pthread_setpriority(long);
void pthread_setdeadline(long);

// Mutex prototypes
int mymutex_init(mutex_t* mutex);
int mymutex_destroy(mutex_t* mutex);
int mymutex_lock(mutex_t* mutex);
int mymutex_unlock(mutex_t* mutex);
int mymutex_trylock(mutex_t* mutex);
