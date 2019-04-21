/*
*******************************************************************
				Instituto Tecnológico de Costa Rica
					Computer Engineering
		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 21/04/2019
					Operating Systems Principles
					Professor. Diego Vargas
*******************************************************************
*/

#include <stdio.h>
#include <mypthread.h>

// Global variables
pthread_t tid[3]; 
int counter;
unsigned long i = 0;
mutex_t* mutex;
mutex_t* mutex2;

void* trythis(void *arg) {
#ifdef MYPTHREAD
	int argInt = arg;
	if (mymutex_trylock(&mutex2) == 0){ // does not blocked the thread if the lock is already locked
#endif
		i = 0;
		counter += 1;
		printf("\n Job %d has started\n\n", counter);
		
		int k = 0;
		for(i; i<(0xEEEEEEE);i++){
			k++;

		}
		printf("k = %d and i = %d, from thread %d\n", k, i, argInt);
		printf("\n Job %d has finished\n\n", counter);
#ifdef MYPTHREAD
		mymutex_unlock(&mutex2);

	} else {
		printf("Error: Mutex locked (thread %d)\n", argInt);

	}
#endif
	return NULL; 
}

void* dothis(void *arg) {
#ifdef MYPTHREAD
	mymutex_lock(&mutex2);
#endif
	i = 0 ;
	counter += 1;
	printf("\n Job %d has started\n\n", counter);
	int argInt = arg;
	int k = 0;
	for(i; i<(0xEEEEEEE);i++){
		k++;

	}
	printf("k = %d and i = %d, from thread %d\n", k, i, argInt);
	printf("\n Job %d has finished\n\n", counter);
#ifdef MYPTHREAD
	mymutex_unlock(&mutex2);
#endif
	return NULL; 
}

/*
 *	mymutex testing
 */
int main(int argc, const char* argv[]){
	int i = 0; 
	int error;
#ifdef MYPTHREAD
	// Set the scheduler algorithm 
	pthread_setsched(SRR);
	// Initialize the mutex
	mymutex_init(&mutex);
	mymutex_init(&mutex2);
#endif
	int thread = 0;
	while(thread < 3) { // creates 3 threads
		if(thread == 1){
			error = pthread_create(&(tid[thread]), NULL, &trythis, thread+1); 
			if (error != 0) 
				printf("\nThread can't be created : [%s]", strerror(error));

		} else {
			error = pthread_create(&(tid[thread]), NULL, &dothis, thread+1); 
			if (error != 0) 
				printf("\nThread can't be created : [%s]", strerror(error)); 

		}
		thread++;
	}
	// Waits the threads to finish
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	return 0;
}