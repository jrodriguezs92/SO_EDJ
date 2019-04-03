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

#include <mypthread.h>

static bool turn;

void *threadedFun(void* args){
	printf("> Hi from thread %d tho\n", (int*)args);
	while(true){
		while(turn){}
		printf("+ Swap turn\n");
		turn=true;
	}
	return 0;
}

void *threadedFun2(void* args){
	printf("> Hi from thread %d tho\n", (int*)args);
	while(true){
		while(!turn){}
		printf("* Swap turn\n");
		turn=false;
	}
	return 0;
}
int main(){
	printf("> Hello from main()\n");
	int t;
	turn=true;

	pthread_t *threads = malloc(sizeof(pthread_t) * 2);
	pthread_create(&threads[0], NULL, threadedFun, (void *) 1); // creates a thread of threadedFun
	pthread_create(&threads[1], NULL, threadedFun2, (void *) 2); // creates a thread of threadedFun2
	
	// Join the created threads
	for (t = 0; t < 2; t++)	{
		pthread_t id = threads[t];
		while (1) {
		    if (pthread_join(id, NULL) >= 0) {
				break;
		    }
		}

	}
	return 0;
}
