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

#include <structs.h>

static int next_id;

/*
 *	To keep the uniqueness of the identifiers
 */
TCB* getNewTCB(void){
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
void destroyTCB(TCB* block){
	if (block->has_dynamic_stack){
		free(block->context.uc_stack.ss_sp);

	}

	free(block);
}


/*	
 *	Create a new initialized QUEUE on the heap. Returns a pointer
 *	to the new block or NULL on error. 
 */
QUEUE* newQUEUE(void){
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
void destroyQUEUE(QUEUE* queue){
	struct NODE *prev = NULL;
	struct NODE *cursor = queue->head;

	while (cursor != NULL) {
		prev = cursor;
		cursor = cursor->next;

		destroyTCB(prev->thread);
		free(prev);

	}

	free(queue);
}


/* 
 *	Return the number of items in queue. 
 */
size_t sizeQUEUE(const QUEUE *queue){
	return queue->size;
}

/*
 *	Add elem to the end of queue. Returns 0 on succes and non-zero
 *	on failure.
 */
int enqueueTCB(QUEUE* queue, TCB *elem){
	// Create the new NODE

	struct NODE *new;

	if ((new = malloc(sizeof(struct NODE))) == NULL) {
		return errno;

	}

	new->thread = elem;
	new->next = NULL;

	// Enqueue the new NODE

	if (queue->head == NULL) {
		queue->head = new;

	} else {
		struct NODE *parent = queue->head;
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
TCB* dequeueTCB(QUEUE* queue){
	struct NODE *old_head = queue->head;
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
TCB* removeByID(QUEUE* queue, int id){
	if (queue->head == NULL) {
		return NULL;

	}

	struct NODE *prev = NULL;
	struct NODE *cur = queue->head;

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