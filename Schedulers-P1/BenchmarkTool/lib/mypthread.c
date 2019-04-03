#include <mypthread.h>

/*
 *	To keep the uniqueness of the identifiers
 */
TCB* tcb_new(void){
	static int next_id;
	TCB *new;
	if ((new = calloc(1, sizeof(TCB))) == NULL) {
		return NULL;

	}

	new->id = next_id++;
	return new;

}

/*
 *	Release the memory of the given TCB
 */
void tcb_destroy(TCB* block){
	if (block->has_dynamic_stack){
		free(block->context.uc_stack.ss_sp);

	}

	free(block);
}


/*	
 *	Create a new initialized QUEUE on the heap. Returns a pointer
 *	to the new block or NULL on error. 
 */
QUEUE* queue_new(void){
	QUEUE* new;

	if ((new = calloc(1, sizeof(QUEUE))) == NULL) {
		return NULL;

	}

	return new;
}


/*
 *	Destroy queue, freeing all associated memory with it. It also
 *	frees all memory of the elements inside the queue.
 */
void queue_destroy(QUEUE* queue){
	struct node *prev = NULL;
	struct node *cursor = queue->head;

	while (cursor != NULL) {
		prev = cursor;
		cursor = cursor->next;

		tcb_destroy(prev->thread);
		free(prev);

	}

	free(queue);
}


/* 
 *	Return the number of items in queue. 
 */
size_t queue_size(const QUEUE *queue){
	return queue->size;
}

/*
 *	Add elem to the end of queue. Returns 0 on succes and non-zero
 *	on failure.
 */
int queue_enqueue(QUEUE* queue, TCB *elem){
	// Create the new node

	struct node *new;

	if ((new = malloc(sizeof(struct node))) == NULL) {
		return errno;

	}

	new->thread = elem;
	new->next = NULL;

	// Enqueue the new node

	if (queue->head == NULL) {
		queue->head = new;

	} else {
		struct node *parent = queue->head;
		while (parent->next != NULL) {
			parent = parent->next;

		}
		parent->next = new;

	}

	queue->size += 1;
	return 0;
}


/*
 *	Remove the first item from the queue and return it. The caller
 *	will have to free the reuturned element. Returns NULL if the
 *	queue is empty. 
 */
TCB* queue_dequeue(QUEUE* queue){
	struct node *old_head = queue->head;
	queue->head = queue->head->next;
	queue->size -= 1;

	TCB *retval = old_head->thread;
	free(old_head);

	return retval;
}


/*
 *	Remove element with matching id from the queue. Returns the
 *	removed element or NULL if no such element exists. 
 */
TCB* queue_remove_id(QUEUE* queue, int id){
	if (queue->head == NULL) {
		return NULL;

	}

	struct node *prev = NULL;
	struct node *cur = queue->head;

	while (cur != NULL) {
		if (cur->thread->id == id) {
			if (prev == NULL) {
				queue->head = cur->next;

			} else {
				prev->next = cur->next;

			}

			TCB *retval = cur->thread;
			free(cur);
			return retval;
		}

		prev = cur;
		cur = cur->next;

	}

	return NULL;
}

/*
 *	Every 10000 µs of execution (thread) a SIGPROF signal is emitted
 *	This is like the quantum
 */
static bool init_profiling_timer(void){
	// Install signal handler

	sigset_t all;
	sigfillset(&all);

	const struct sigaction alarm = {
		.sa_sigaction = handle_sigprof,
		.sa_mask = all,
		.sa_flags = SA_SIGINFO | SA_RESTART
	};

	struct sigaction old;

	if (sigaction(SIGPROF, &alarm, &old) == -1) {
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
 *	Handles the signal produced by the timer
 *	Makes the scheduling
 */
static void handle_sigprof(int signum, siginfo_t *nfo, void *context){
	int old_errno = errno;

	// Backup the current context

	ucontext_t *stored = &running->context;
	ucontext_t *updated = (ucontext_t *) context;

	stored->uc_flags = updated->uc_flags;
	stored->uc_link = updated->uc_link;
	stored->uc_mcontext = updated->uc_mcontext;
	stored->uc_sigmask = updated->uc_sigmask;

	// Round robin

	if (queue_enqueue(ready, running) != 0) {
		perror("queue_enqueue()");
		abort();

	}

	if ((running = queue_dequeue(ready)) == NULL) {
		perror("queue_dequeue()");
		abort();

	}

	// Manually leave the signal handler

	errno = old_errno;
	if (setcontext(&running->context) == -1) {
		perror("setcontext()");
		abort();

	}
}

/*
 *	To block SIGPROF in some functions and avoid the queues to get corrupted
 */
static void block_sigprof(void){
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
static void unblock_sigprof(void){
	sigset_t sigprof;
	sigemptyset(&sigprof);
	sigaddset(&sigprof, SIGPROF);

	if (sigprocmask(SIG_UNBLOCK, &sigprof, NULL) == -1) {
		perror("sigprocmask(SIG_UNBLOCK)");
		abort();
	}
}

/*
 *	Initialize the queues
 */
static bool init_queues(void){
	if ((ready = queue_new()) == NULL) {
		return false;
	}

	if ((completed = queue_new()) == NULL) {
		queue_destroy(ready);
		return false;
	}

	return true;
}

/*
 *	Currently running context is backed up in a thread control block (first thread).
 */
static bool init_first_context(void){
	TCB *block;

	if ((block = tcb_new()) == NULL) {
		return false;
	}

	if (getcontext(&block->context) == -1) {
		tcb_destroy(block);
		return false;
	}

	running = block;
	return true;
}

/*
 *	Creates a thread
 */
int pthread_create(pthread_t* thread, void* attr, void *(*start_routine) (void *), void *arg){
	block_sigprof(); //blocks the sigprof

	//static bool initialized;

	// Initialization
	if (! initialized) {
		// inits the queues
		if (! init_queues()) {
			abort();

		}

		// sets the first conext (main)
		if (! init_first_context()) {
			abort();

		}

		// starts the timer
		if (! init_profiling_timer()) {
			abort();

		}

		initialized = true;

	}

	// Create a thread control block for the newly created thread.
	TCB* new;

	if ((new = tcb_new()) == NULL) {
		return -1;

	}

	if (getcontext(&new->context) == -1) {
		tcb_destroy(new);
		return -1;

	}

	if (! malloc_stack(new)) {
		tcb_destroy(new);
		return -1;

	}

	makecontext(&new->context, handle_thread_start, 1, new->id);

	new->start_routine = start_routine;
	new->argument = arg;

	// Enqueue the newly created stack
	if (queue_enqueue(ready, new) != 0) {
		tcb_destroy(new);
		return -1;

	}

	unblock_sigprof();
	*thread = new->id;
	return new->id; // returns the thread id
}

/*
 *	Allocates memory on the heap later used as a stack. 
 *	Then modifies the context stored in the thread control 
 *	block to include that stack.
 */
static bool malloc_stack(TCB* thread){
	// Get the stack size

	struct rlimit limit;

	if (getrlimit(RLIMIT_STACK, &limit) == -1) {
		return false;

	}

	// Allocate memory
	void *stack;

	if ((stack = malloc(limit.rlim_cur)) == NULL) {
		return false;

	}

	// Update the thread control block
	thread->context.uc_stack.ss_flags = 0; // new stack
	thread->context.uc_stack.ss_size = limit.rlim_cur;
	thread->context.uc_stack.ss_sp = stack;
	thread->has_dynamic_stack = true;

	return true;
}

/*
 *	A wrapper for set funtion of TCB. It calls threads_exit to 
 *	guaranteed that every thread creadte calls threads_exit, 
 *	even if the thread only returns.
 */
static void handle_thread_start(void){
	block_sigprof();
	TCB *this = running;
	unblock_sigprof();

	void *result = this->start_routine(this->argument);
	//threads_exit(result);
}

/*
 *	Enqueues the currently running thread control block in the 
 *	completed queue. It then yields the processor to the next thread in line.
 */
void pthread_exit(void *result){
	if (running == NULL) {
		exit(EXIT_SUCCESS);

	}

	block_sigprof();

	running->return_value = result;

	if (queue_enqueue(completed, running) != 0) {
		abort();

	}

	if ((running = queue_dequeue(ready)) == NULL) {
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

	block_sigprof();
	TCB *block = queue_remove_id(completed, id);
	unblock_sigprof();

	if (block == NULL) {
		return -1;

	} else {
		if (result!=NULL){
			*result = block->return_value;
		}
		tcb_destroy(block);
		return id;

	}
}