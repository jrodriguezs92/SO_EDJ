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
static QUEUE* ready; // threads awaiting CPU / Will be the accepted queue for SRR
static QUEUE* completed;
static QUEUE* new;
static TCB* running; // current thread
static LIST* tickets; // for LOTTERY algorithm
static bool initialized;
static int sched = LOTTERY; // selfish round robin by default
static int acceptedPriority; // max priority on accepted queue

// Preemptive related prototypes
static void blockSIGPROF(void);
static void unblockSIGPROF(void);
static void scheduleHandler(int signum, siginfo_t *nfo, void *context);
static bool signalTimer(void);
static bool updatePriorities(void);
static int lotteryDraw(void);

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

	if ((tickets = newLIST()) == NULL) {
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
	acceptedPriority = 1; // initialize the accepted priority

	if (sched == LOTTERY){
		if (enqueueTCB(ready, running) != 0) {
			perror("enqueueTCB()");
			abort();
		}
		pthread_setpriority(786432001);
	}

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
	} 
	// Selfish Round Robin
	else if(sched == SRR) {
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
	// Lottery
	else if(sched == LOTTERY) {
		int winner = lotteryDraw();

		int idWinner;
		if ((idWinner = getByIndex(tickets,winner)) == -1){
			perror("dequeueTCB()");
			abort();
		}

		TCB* nextToRun;
		if ((nextToRun = getByID(ready,idWinner)) == NULL){
			perror("dequeueTCB()");
			abort();
		}

		running = nextToRun;
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
	bool temp = false;

	if( new->size == 0 && ready->size == 0 ){
		running->priority = 1; // case the main thread is the only one running (avoid starvation)
		return temp;

	}

	if(new->size != 0){
		// Iterate on the the new queue
		for(cursor = new->head ; cursor != NULL ; cursor = cursor->next){
			cursor->thread->priority += PA;

		}

		acceptedPriority += PB; // priority+=b, ONLY if there is a thread in the new queue

		if(acceptedPriority > MAX_PRIORITY){
			acceptedPriority = 1; // resets the priority
		}

		if(new->head->thread->priority >= acceptedPriority){
			temp = true;
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
	} else if(sched == LOTTERY){
		// Enqueue the newly created stack
		if (enqueueTCB(ready, newThread) != 0) {
			destroyTCB(newThread);
			return -1;

		}
		addTicket(tickets,newThread->id);
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

	if (sched == LOTTERY){
		// Remove corresponding tickets
		int result = removeTicket(tickets,running->id);
		while (result != 0) {
			result = removeTicket(tickets, running->id);
		}

		removeByID(ready, running->id);

		int winner = lotteryDraw();
		int idWinner = getByIndex(tickets,winner);
		TCB* nextToRun = getByID(ready,idWinner);
		running = nextToRun;
	}
	else {
		if ((running = dequeueTCB(ready)) == NULL) {
			exit(EXIT_SUCCESS);
		}
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

/**
 * Set the real priority for thread and set the lottery tickets
 * (for LOTTERY schedule)
 */
void pthread_setpriority(long fSize){

	if (sched != LOTTERY){return;}

	int priority;
	
	// Set priority according to the requested file size
	if (fSize>=0 && fSize<262144000){
		priority = 1;
	}
	else if (fSize>=262144001 && fSize<524288000){
		priority = 2;
	}
	else if (fSize>=524288001 && fSize<786432000){
		priority = 3;
	}
	else if (fSize>=786432001 && fSize<1073741824){
		priority = 4;
	}
	else if (fSize>=1073741825 && fSize<1572864000){
		priority = 5;
	}
	else if (fSize>=1572864001){
		priority = 6;
	}

	// Update the lottery tickets
	int tmp=1;
	while(tmp<=priority){

		if (addTicket(tickets,running->id) != 0){
			perror("enqueueTCB()");
			abort();
		}

		tmp++;
	}
}

/**
 * Make lottery draw to obtain the TCB's id winner
 */
static int lotteryDraw(void){
	size_t LISTSize = tickets->size;
	int topBound = ((int) LISTSize);

	// Get random pos (winner ticket)
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	/* using nano-seconds instead of seconds */
	srand((time_t)ts.tv_nsec);
	int win = rand()%topBound;
	return win;
}