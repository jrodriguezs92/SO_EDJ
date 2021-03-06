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
	clock_t start = clock();
	printf("THREAD %d\n", args);
#ifdef MYPTHREAD
	pthread_setdeadline(1048576); // 1MB
#endif
	int i=0;
	int it=0;
	while(1){
		i+=1;
		it++;
		if(i==1234000001){
			printf("ITERACIONES THREAD %d= %d\n",args, it );
			clock_t end = clock();
			double total = (double) (end-start)/CLOCKS_PER_SEC;
			printf("> TIME FUNC1= %lfs\n", total );
			return i;

		}
	}
}

void* func2(void* args){
	printf("THREAD %d\n", args);
	int i=0;
	int it=0;
	while(1){
		i+=2;
		it++;
		if(i==246800000){
			printf("ITERACIONES THREAD %d= %d\n",args, it );
			return i;

		}
	}
}

void* func3(void* args){
	printf("THREAD %d\n", args);
	int i=0;
	int it=0;
	while(1){
		i+=3;
		it++;
		if(i==370200000){
			printf("ITERACIONES THREAD %d= %d\n",args, it );
			return i;
			
		}
		
	}
}

int main(int argc, const char* argv[]){
	// defines the scheduler/ if no mypthread no problem
#ifdef MYPTHREAD
	int sch = SRR;
	if(argc==2){
		sch = atoi(argv[1]);
	}
	pthread_setsched(sch);
	printf("> scheduler = %d\n", sch);
#endif
	clock_t start = clock();

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