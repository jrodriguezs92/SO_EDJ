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

int main(int argc, char *argv[]){
	// Parsing of the arguments
	int n_threads;
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
		rc = pthread_create(&threads[t], NULL, sendRequest, (void *) t);
		if (rc) {
			perror("pthread_create()");
			exit(-1);

		}
	}

	// Detached the created threads
	for (t = 0; t < n_threads; t++)	{
		pthread_detach(threads[t]);

	}

	pthread_exit(NULL);
	return 0;
}

/*bclient.c*/