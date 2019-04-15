/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 05/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <mypthread.h>

// Global variables
static QUEUE* ready; // threads awaiting CPU
static QUEUE* completed;
static QUEUE* new;
static TCB* running; // current thread
static bool initialized;
static int sched = SRR; // selfish round robin by default

// Preemptive related prototypes
static void blockSIGPROF(void);
static void unblockSIGPROF(void);
static void scheduleHandler(int signum, siginfo_t *nfo, void *context);
static bool signalTimer(void);
static bool updatePriorities(void);

// Threads related prototypes
static bool initQueues(void);
static bool initFirstContext(void);
static bool setStackTCB(TCB*);
static void handleThreadFunction(void);

/*
 *	Initialize the queues
 */
static bool initQueues(void){
	if ((ready = newQUEUE()) == NULL) {
		return false;
	}
	if ((completed = newQUEUE()) == NULL) {
		destroyQUEUE(ready);
		return false;
	}
	if ((new = newQUEUE()) == NULL) {
		return false;
	}

	return true;
}

/*
 *	Currently running context is backed up in a thread control block (first thread).
 */
static bool initFirstContext(void){
	TCB *block;

	if ((block = getNewTCB()) == NULL) {
		return false;
	}

	if (getcontext(&block->context) == -1) {
		destroyTCB(block);
		return false;
	}
	block->priority = 1;
	running = block;
	return true;
}

/*
 *	Allocates memory on the heap later used as a stack. 
 *	Then modifies the context stored in the thread control 
 *	block to include that stack.
 */
static bool setStackTCB(TCB* thread){
	// Allocate memory
	void *stack;

	if ((stack = malloc(STACK_SIZE)) == NULL) {
		return false;

	}

	// Update the thread control block
	thread->context.uc_stack.ss_size = STACK_SIZE;
	thread->context.uc_stack.ss_sp = stack;
	thread->has_dynamic_stack = false; // fixed stack

	return true;
}

/*
 *	A wrapper for set funtion of TCB. It calls threads_exit to 
 *	guaranteed that every thread creadte calls threads_exit, 
 *	even if the thread only returns.
 */
static void handleThreadFunction(void){
	blockSIGPROF(); // blocks the sigprof
	TCB *this = running;
	unblockSIGPROF(); // unblocks the sigprof
	void *result = this->start_routine(this->argument);
	pthread_exit(result);
}

/*
 *	To block SIGPROF in some functions and avoid the queues to get corrupted
 */
static void blockSIGPROF(void){
	sigset_t sigprof;
	sigemptyset(&sigprof);
	sigaddset(&sigprof, SIGPROF);

	if (sigprocmask(SIG_BLOCK, &sigprof, NULL) == -1) {
		perror("sigprocmask(SIG_BLOCK)");
		abort();
	}
}

/*
 *	To unblock SIGPROF
 */
static void unblockSIGPROF(void){
	sigset_t sigprof;
	sigemptyset(&sigprof);
	sigaddset(&sigprof, SIGPROF);

	if (sigprocmask(SIG_UNBLOCK, &sigprof, NULL) == -1) {
		perror("sigprocmask(SIG_UNBLOCK)");
		abort();
	}
}

/*
 *	Handles the signal produced by the timer
 *	Makes the scheduling
 */
static void scheduleHandler(int signum, siginfo_t *nfo, void *context){
	int old_errno = errno;

	// Backup the current context
	ucontext_t* stored = &running->context;

	// Round robin
	if(sched == RR){
		if (enqueueTCB(ready, running) != 0) {
			perror("enqueueTCB()");
			abort();

		}

		if ((running = dequeueTCB(ready)) == NULL) {
			perror("dequeueTCB()");
			abort();

		}
	} else if(sched == SRR) {
		if(updatePriorities()){
			TCB* newAccepted;
			if ((newAccepted = dequeueTCB(new)) == NULL) {
				perror("dequeueTCB()");
				abort();

			}

			if (enqueueTCB(ready, newAccepted) != 0) {
				perror("enqueueTCB()");
				abort();

			}

			if (enqueueTCB(ready, running) != 0) {
				perror("enqueueTCB()");
				abort();

			}
			if ((running = dequeueTCB(ready)) == NULL) {
				perror("dequeueTCB()");
				abort();

			}

		} else{
			if (enqueueTCB(ready, running) != 0) {
				perror("enqueueTCB()");
				abort();

			}

			if ((running = dequeueTCB(ready)) == NULL) {
				perror("dequeueTCB()");
				abort();

			}
		}
	}

	// Manually leave the signal handler
	errno = old_errno;

	if (swapcontext(stored, &(running->context)) == -1) {
		perror("swapcontext()");
		abort();

	}
}

/*
 *	Every 10000 µs of execution (thread) a SIGPROF signal is emitted
 *	This is like the quantum
 */
static bool signalTimer(void){
	// Install signal handler
	sigset_t all;
	sigfillset(&all);

	const struct sigaction quantum = {
		.sa_sigaction = scheduleHandler,
		.sa_mask = all,
		.sa_flags = SA_SIGINFO | SA_RESTART
	};

	struct sigaction old;

	if (sigaction(SIGPROF, &quantum, &old) == -1) {
		perror("sigaction()");
		abort();
	}

	const struct itimerval timer = {
		{ 0, 10000 }, // 10000 µs
		{ 0, 1 }  // arms the timer as soon as possible
	};

	// Enable timer
	if (setitimer(ITIMER_PROF, &timer, NULL) == - 1) {
		if (sigaction(SIGPROF, &old, NULL) == -1) {
			perror("sigaction()");
			abort();
		}

		return false;
	}

	return true;
}

/*
 *	Updates the priorities in general, returns if new priority (head)is 
 *	equal to the running priority (move the head from new to ready/accepted)
 */
static bool updatePriorities(void){
	struct NODE* cursor;
	bool temp =false;
	if(new->size==0 && ready->size==0){
		running->priority = 0; // case the main thread is the only one running (avoid starvation)
		return temp;

	}

	if(new->size!=0){
		// Iterate on the the new queue
		for(cursor = new->head ; cursor != NULL ; cursor = cursor->next){
			cursor->thread->priority += PA;

		}

		running->priority += PB; // priority+=b, only if there is a thread in the new queue
		if(running->priority > MAX_PRIORITY){
			running->priority = 1; // resets the priority
		}
		if(new->head->thread->priority >= running->priority){
			running->priority = new->head->thread->priority;
			temp = true;
		}

	}

	if(ready->size!=0){
		// Iterate on the the ready/accepted queue
		for(cursor = ready->head ; cursor != NULL ; cursor = cursor->next){
			// All the priorities in ready are the same
			cursor->thread->priority = running->priority;
			
		}

	}
	return temp;
}

/*
 *	Creates a thread
 */
int pthread_create(pthread_t* thread, void* attr, void *(*start_routine) (void *), void *arg){
	blockSIGPROF(); //blocks the sigprof
	// Initialization
	if (! initialized) {
		// inits the queues
		if (! initQueues()) {
			abort();

		}

		// sets the first conext (main)
		if (! initFirstContext()) {
			abort();

		}

		if (! signalTimer()){
			abort();
		}

		initialized = true;

	}

	// Create a thread control block for the newly created thread.
	TCB* newThread;

	if ((newThread = getNewTCB()) == NULL) {
		return -1;

	}

	if (getcontext(&newThread->context) == -1) {
		destroyTCB(newThread);
		return -1;

	}

	if (! setStackTCB(newThread)) {
		destroyTCB(newThread);
		return -1;

	}

	makecontext(&newThread->context, handleThreadFunction, 1, newThread->id);

	newThread->start_routine = start_routine;
	newThread->argument = arg;
	newThread->priority = 0;

	if(sched == RR){
		// Enqueue the newly created stack
		if (enqueueTCB(ready, newThread) != 0) {
			destroyTCB(newThread);
			return -1;

		}
	} else if(sched == SRR){
		// Enqueue the newly created stack
		if (enqueueTCB(new, newThread) != 0) {
			destroyTCB(newThread);
			return -1;

		}
	}

	unblockSIGPROF(); // unblocks the sigprof
	*thread = newThread->id; // sets the id
	return 0; // returns the succes
}

/*
 *	Enqueues the currently running thread control block in the 
 *	completed queue. It then yields the processor to the next thread in line.
 */
void pthread_exit(void *result){
	if (running == NULL) {
		exit(EXIT_SUCCESS);

	}

	blockSIGPROF(); // blocks the sigprof

	running->return_value = result;

	if (enqueueTCB(completed, running) != 0) {
		abort();

	}

	if ((running = dequeueTCB(ready)) == NULL) {
		exit(EXIT_SUCCESS);

	}

	setcontext(&running->context);  // also unblocks SIGPROF
}

/*
 *	Looks through the queue completed, if an element with 
 *	matching id is found, that thread control block is 
 *	destroyed for good. The return value is written into result.
 */
int pthread_join(pthread_t id, void **result){
	if (id < 0) {
		errno = EINVAL;
		return -1;

	}
	while(true){ // to wait until the thread finished
		blockSIGPROF();
		TCB *block = removeByID(completed, id);
		unblockSIGPROF();

		if (block != NULL) { // finished
			if (result!=NULL){
				*result = block->return_value;
			}
			destroyTCB(block);
			return id;
		}
	}
}

/**
 *	Yields the CPU to another thread
 */
int pthread_yield(void){
	// Backup the current context

	ucontext_t *stored = &running->context;

	// Round robin

	if (enqueueTCB(ready, running) != 0) {
		perror("enqueueTCB()");
		abort();

	}

	if ((running = dequeueTCB(ready)) == NULL) {
		perror("dequeueTCB()");
		abort();

	}

	swapcontext(stored, &(running->context));
	return 0;
}

/**
 *	Sets the scheduler algoritm
 */
void pthread_setsched(int schedAlgoritm){
	sched = schedAlgoritm;

}