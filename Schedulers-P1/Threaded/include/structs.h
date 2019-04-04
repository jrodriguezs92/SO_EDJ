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
#include <errno.h>
#include <ucontext.h>
#include <stdbool.h>

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
typedef struct {
	struct NODE *head;
	size_t size;
} QUEUE;

struct NODE {
	TCB *thread;
	struct NODE *next;
};

// TCB related prototypes
TCB* getNewTCB(void);
void destroyTCB(TCB*);

// QUEUE related prototypes
QUEUE* newQUEUE(void);
void destroyQUEUE(QUEUE*);
size_t sizeQUEUE(const QUEUE*);
int enqueueTCB(QUEUE*, TCB*);
TCB* dequeueTCB(QUEUE*);
TCB* removeByID(QUEUE*, int);