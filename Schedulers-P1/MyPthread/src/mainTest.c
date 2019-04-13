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
#include <time.h>

void* func1(void* args){
	printf("THREAD %d\n", args);
	int i=0;
	int it=0;
	while(1){
		//printf("Executing within func1\n");
		i+=1;
		//yield();
		//printf("Func1 Value - %d\n", i);
		it++;
		if(i==123400000){
			printf("ITERACIONES = %d\n", it );
			return i;
		}
	}
}

void* func2(void* args){
	printf("THREAD %d\n", args);
	int i=0;
	int it=0;
	while(1){
		//printf("Executing within func2\n");
		i+=2;
		//yield();
		//printf("Func2 Value - %d\n", i);
		it++;
		if(i==246800000){
			printf("ITERACIONES = %d\n", it );
			return i;
		}
	}
}

void* func3(void* args){
	printf("THREAD %d\n", args);
	int i=0;
	int it=0;
	while(1){
		//printf("Executing within func3\n");
		i+=3;
		//yield();
		//printf("Func3 Value - %d\n", i);
		it++;
		if(i==370200000){
			printf("ITERACIONES = %d\n", it );
			return i;
		}
		
	}
}

int main(){
	clock_t start = clock();

// defines the scheduler/ if no mypthread no problem
#ifdef MYPTHREAD
	pthread_setsched(SRR);
	printf("Using algoritm %d\n", RR );
#endif

	pthread_t* threads = malloc(sizeof(pthread_t)*4);
	pthread_create(&threads[0],NULL,func1,1);
	pthread_create(&threads[1],NULL,func2,2);
	pthread_create(&threads[2],NULL,func3,3);

	int* results = malloc(sizeof(int)*3);

	// joins the threads
	int i;
	for(i=0; i<3; i++){
		if(i==2){
			pthread_create(&threads[3],NULL,func3,4);
		}
		pthread_join(threads[i],&results[i]);
		//sleep(1);
	}



	
	pthread_join(threads[3],&results[3]);

	printf("Result From 1 = %d\n", results[0]);
	printf("Result From 2 = %d\n", results[1]);	
	printf("Result From 3 = %d\n", results[2]);
	printf("Result From 4 = %d\n", results[3]);
	clock_t end = clock();
	double total = (double) (end-start)/CLOCKS_PER_SEC;
	printf("> TIME = %lfs\n", total );
	return 0;
}
