/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 02/04/2019

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

// Thread Control Block, to keep the track of all threads
typedef struct {
	int id; // identifier
	ucontext_t context; // context of the thread
	bool has_dynamic_stack; // context of the thread
	void *(*start_routine) (void *); // function to be executed by the thread
	void *argument; // argument of the function
	void *return_value; // return of the function
} TCB;

// Basic Data Structure to keep track of the TCBs
typedef struct queue QUEUE;
struct node {
	TCB *thread;
	struct node *next;
};
struct queue {
	struct node *head;
	size_t size;
};

// Global variables
static QUEUE* ready; // threads awaiting CPU
static QUEUE* completed;
static TCB* running; // current thread
static bool initialized;

// TCB related prototypes
TCB* tcb_new(void);
void tcb_destroy(TCB*);

// QUEUE related prototypes
QUEUE* queue_new(void);
void queue_destroy(QUEUE*);
size_t queue_size(const QUEUE*);
int queue_enqueue(QUEUE*, TCB*);
TCB* queue_dequeue(QUEUE*);
TCB* queue_remove_id(QUEUE*, int);

// Preemptive related prototypes
static bool init_profiling_timer(void);
static void handle_sigprof(int, siginfo_t*, void*);
static void block_sigprof(void);
static void unblock_sigprof(void);

// Threads related prototypes
static bool init_queues(void);
static bool init_first_context(void);
int pthread_create(pthread_t*, void*, void *(*start_routine) (void *), void *arg);
static bool malloc_stack(TCB*);
static void handle_thread_start(void);
void pthread_exit(void*);
int pthread_join(pthread_t id, void **result);