/*bclient.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernandez A (DaniHdez)
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

/*Get extension*/
char *getExt(char *str) {
    char *result;
    char *last;
    if ((last = strrchr(str, '.')) != NULL) {
        if ((*last == '.') && (last == str))
            return "";
        else {
            result = (char*) malloc(MAX_FILENAME_SIZE);
            snprintf(result, sizeof result, "%s", last + 1);
            return result;
        }
    } else {
        return ""; // Empty/NULL string
    }
}
/*Create the file CSV*/
int createCSV(int port, int threads, int cycles, char * reqTimeI, char * reqTimeF, float rTime ,char * typeFile, int fileSize, int average, int first){

	char * webserverType;
	int reqNumber;
	double averageCalc;
	const char s[2] = "\n";
   	char *tokenIni;
   	char *tokenFin;
   	char *fileName;

   	if (first == 1){
   		tempTime = malloc(strlen(reqTimeI)+1);
   		strcpy(tempTime, reqTimeI);
   	}

   	fileName = malloc(strlen("results/ExeReport_")+strlen(tempTime)+strlen(".csv")+1);

   	strcpy(fileName,"results/ExeReport_");
   	strcat(fileName, tempTime);
   	strcat(fileName,".csv");
   	
	FILE * results;
	results = fopen(fileName,"a");

	if (port==8001){
		webserverType = "FIFO";
	}
	else if (port==8003){
		webserverType = "Forked";
	}
	else if (port==8005){
		webserverType = "Threaded";
	}
	else if (port==8007){
		webserverType = "Prethreaded";
	}
	else if (port==8009){
		webserverType = "Preforked";
	}
	else
		webserverType="Not define";

	/*Request number*/
	reqNumber=threads*cycles;


   /* get the first token*/
   tokenIni = strtok(reqTimeI, s);
   tokenFin = strtok(reqTimeF,s);

	double timeTaken = ((double)rTime)/CLOCKS_PER_SEC;

	/*Calculate average*/
	if (average==1)
	{
		double sum;
		for (int i = 0; i < threads; ++i)
		{
			double timeTaken = ((double)responseTime[i])/CLOCKS_PER_SEC;
			sum += timeTaken; 
		}
		averageCalc= sum/threads;
		fprintf(results, "\nComplete execution" );
		fprintf(results,"\nWeb Server Type,Request number,Initial request time,Final request time, File type, File size (bytes), Response time, Average time");
		fprintf(results, "\n%s,%d,%s,%s,%s,%d,%f,%f\n",webserverType,reqNumber,tokenIni,tokenFin,typeFile,fileSize,timeTaken,averageCalc);
	}
	else {
		averageCalc = 0;
		fprintf(results, "\nThread execution" );
		fprintf(results,"\nWeb Server Type,Request number,Initial request time,Final request time, File type, File size (bytes), Response time");
		fprintf(results, "\n%s,%d,%s,%s,%s,%d,%f,%f\n",webserverType,reqNumber,tokenIni,tokenFin,typeFile,fileSize,timeTaken);
	}
	
	fclose(results);
	return 0;
}

int main(int argc, char *argv[]){
	// Parsing of the arguments
	int n_threads;
	//time
	struct tm *newtime;
    time_t ltime;
    float mainResponseTime;
    clock_t start, end, startMain, endMain;
    startMain=clock();
    char * fileExt;

	if (argc != 6)	{  // If the total arguments are not provided, erro
		printHelp();
		return -1;

	}

	// defines the scheduler/ if no mypthread no problem
#ifdef MYPTHREAD
	pthread_setsched(RR);
#endif

    // Set the values to the corresponding arguments
	host = argv[1];            // ip of the server
	port = atoi(argv[2]);      // port of the server
	file = argv[3];            // file to be transferred
	n_threads = atoi(argv[4]); // number of threads to run
	n_cycles = atoi(argv[5]);  // times to repeat the requests

	/*File extension*/
	fileExt = getExt(file);

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
		
		start = clock();
		initTime[t]=start;

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
		
		end = clock();
		finTime[t]=end;

	}
	if(threads!=NULL){
		free(threads);
	}
	for (int i = 0; i < n_threads; ++i)
	{
		responseTime[i]=finTime[i]-initTime[i];
		createCSV(port, n_threads, n_cycles, initialTime[i], finalTime[i], responseTime[i], fileExt, size,0,1);
	}
	

	printf("> Execution complete\n");
	endMain=clock();
	mainResponseTime = endMain - startMain;
	createCSV(port, n_threads, n_cycles,initialTime[0],finalTime[n_threads-1],mainResponseTime,fileExt, size,1,0);
	return 0;

}

/*bclient.c*/
