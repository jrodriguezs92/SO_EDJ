#include <stdio.h>
#include <mypthread.h>

pthread_t tid[3]; 
int counter;

unsigned long i = 0;

mutex_t* mutex;
mutex_t* mutex2;

void* trythis(void *arg) {

#ifdef MYPTHREAD
	if (mymutex_trylock(&mutex2) == 0){ // does not blocked the thread
#endif
		counter += 1;
		printf("\n Job %d has started\n", counter);
		int argInt = arg;
		int k = 0;
		for(i; i<(0xEEEEEEE);i++){
			k++;
		}
		printf("k = %d and i = %d, from thread %d\n", k, i, argInt);
		printf("\n Job %d has finished\n", counter);
#ifdef MYPTHREAD
		mymutex_unlock(&mutex2);
	} else {
		printf("Error: Mutex locked\n");
	}
#endif

	return NULL; 
}

void* dothis(void *arg) {

#ifdef MYPTHREAD
	mymutex_lock(&mutex2);
#endif
	counter += 1;
	printf("\n Job %d has started\n", counter);
	int argInt = arg;
	int k = 0;
	for(i; i<(0xEEEEEEE);i++){
		k++;
	}
	printf("k = %d and i = %d, from thread %d\n", k, i, argInt);
	printf("\n Job %d has finished\n", counter);

#ifdef MYPTHREAD
	mymutex_unlock(&mutex2);
#endif

	return NULL; 
}

int main(int argc, const char* argv[]){
	int i = 0; 
	int error;
	printf("K1 %d\n", mutex);
	//mutex = malloc(sizeof(mutex_t));
	//mutex2 = malloc(sizeof(mutex_t));
#ifdef MYPTHREAD
	printf("K2\n");
	pthread_setsched(RR);
	printf("K3\n");
	mymutex_init(&mutex);
	mymutex_init(&mutex2);
#endif
	printf("K4 %d\n", mutex);
	int thread = 0;
	while(thread < 3) { 
		if(thread == 1){
			error = pthread_create(&(tid[thread]), NULL, &trythis, thread); 
			if (error != 0) 
				printf("\nThread can't be created : [%s]", strerror(error)); 
		} else {
			error = pthread_create(&(tid[thread]), NULL, &dothis, thread); 
			if (error != 0) 
				printf("\nThread can't be created : [%s]", strerror(error)); 
		}
		thread++;
	}

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	return 0;
}