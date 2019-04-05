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

#include <mypthread.h>

void* func1(void* args){
	int i=0;
	while(1){
		printf("Executing within func1\n");
		i+=1;
		//yield();
		printf("Func1 Value - %d\n", i);
		if(i==1234){
			return i;
		}
	}
}

void* func2(void* args){
	int i=0;
	while(1){
		printf("Executing within func2\n");
		i+=2;
		//yield();
		printf("Func2 Value - %d\n", i);
		if(i==100000){
			return i;
		}
	}
}

void* func3(void* args){
	int i=0;
	while(1){
		printf("Executing within func3\n");
		i+=3;
		//yield();
		printf("Func3 Value - %d\n", i);
		if(i==12){
			return i;
		}
		
	}
}

int main(){
	pthread_t* threads = malloc(sizeof(pthread_t)*3);
	pthread_create(&threads[0],NULL,func1,NULL);
	pthread_create(&threads[1],NULL,func2,NULL);
	pthread_create(&threads[2],NULL,func3,NULL);

	int* results = malloc(sizeof(int)*3);

	// joins the threads
	int i;
	for(i=0; i<3; i++){
		pthread_join(threads[i],&results[i]);
	}

	printf("Result From 1 = %d\n", results[0]);
	printf("Result From 2 = %d\n", results[1]);	
	printf("Result From 3 = %d\n", results[2]);
	return 0;
}
