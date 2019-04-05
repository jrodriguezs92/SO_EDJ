/*bclient.c*/

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
#include <client.h>

void printHelp(){
	printf("> EDJ BenchmarkTool for WebServers <\n");
	printf("> Example: $bclient <machine> <port> <file> <N-threads> <N-cycles>\n> Where:\n");
	printf("\t <machine> is the IP address where the web server is running\n");
	printf("\t <port> is the corresponding port number of web server\n");
	printf("\t <file> file that is going to be transferred (path include\n");
	printf("\t <N-threads> is the amount of threads that are going to send requests web server\n");
	printf("\t <N-cycles> the times that each thread going to repeat the requests\n");
}

int createCSV(int port, int threads, int cycles, char * reqTimeI, char * reqTimeF, char * typeFile){

	char * webserverType;
	int reqNumber;
	float responseTime;

	FILE * results;
	results = fopen("exeReport.csv","a");

	fprintf(results,"\nWeb Server Type,Request number,Initial request time,Final request time, File type, File size, Response time, Average time");
	if (port==8001){
		webserverType = "Threaded";
	}
	else if (port==8002){
		webserverType = "Forked";
	}
	else if (port==8003){
		webserverType = "FIFO";
	}
	else if (port==8003){
		webserverType = "Prethreaded";
	}
	else if (port==8002){
		webserverType = "Preforked";
	}
	else
		webserverType="Not define";

	reqNumber=threads*cycles;
	responseTime = reqTimeF - reqTimeI;

	fprintf(results, "\n%s,%d,%s,%s,%s,NULL,%f,NULL\n",webserverType,reqNumber,reqTimeI,reqTimeF,typeFile,responseTime);

	fclose(results);
	return 0;
}

int main(int argc, char *argv[]){
	// Parsing of the arguments
	int n_threads;
	//time
	struct tm *newtime;
    time_t ltime;
	if (argc != 6)	{  // If the total arguments are not provided, erro
		printHelp();
		return -1;

	}

    // Set the values to the corresponding arguments
	host = argv[1];            // ip of the server
	port = atoi(argv[2]);      // port of the server
	file = argv[3];            // file to be transferred
	n_threads = atoi(argv[4]); // number of threads to run
	n_cycles = atoi(argv[5]);  // times to repeat the requests

	pthread_t *threads = malloc(sizeof(pthread_t) * n_threads);
	int rc;
	long t;

	// Creates the N threads
	for (t = 0; t < n_threads; t++)	{
		
	    /* Get the time in seconds */
	    time(&ltime);
		/* Convert it to the structure tm */
	    newtime = localtime(&ltime);

		strcpy(initialTime[t], asctime(newtime));

		//printf("Starts at %s", initialTime[t]);

		rc = pthread_create(&threads[t], NULL, sendRequest, (void *) t);
		if (rc<0) {
			perror("pthread_create()");
			exit(-1);

		}

	}

	// Join the created threads
	for (t = 0; t < n_threads; t++)	{
		pthread_t id = threads[t];
		pthread_join(id, NULL);
		

	    /* Get the time in seconds */
	    time(&ltime);
		/* Convert it to the structure tm */
	    newtime = localtime(&ltime);

		strcpy(finalTime[t], asctime(newtime));

		//printf("Ends at %s", finalTime[t]);
	}
	if(threads!=NULL){
		free(threads);
	}
	for (int i = 0; i < n_threads; ++i)
	{
		createCSV(port, n_threads, n_cycles, initialTime[i], finalTime[i], file);
	}
	
	printf("> Execution complete\n");
	return 0;
}

/*bclient.c*/