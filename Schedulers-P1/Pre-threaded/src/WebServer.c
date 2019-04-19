/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernández A (DaniHdez)

		Programming Language: C
		Version: 1.0
		Last Update: 03/04/2019

		Inputs: Port and/ or root directory
		Restrictions: Numeric Value / String

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;

/**
 * main thread of the server
 */
int main(int argc, char* argv[]){
	static struct option long_options[] = {
		{"conf_file", required_argument, 0, 'c'},
		{"test_conf", required_argument, 0, 't'},
		{"log_file", required_argument, 0, 'l'},
		{"help", no_argument, 0, 'h'},
		{"daemon", no_argument, 0, 'd'},
		{"pid_file", required_argument, 0, 'p'},
		{NULL, 0, 0, 0}
	};

	int value, optionIndex = 0;
	logFileName = (char *)malloc(50*sizeof(char));
	logFileName[0] = '\0';
	startDaemonized = 0;
	running = 0;
	confFileName = NULL;
	pidFileName = NULL;
	pidFd = -1;
	appName = NULL;

	// server

	pthread_mutex_init(&clifd_mutex, NULL);
	
	port = (char*)malloc(10 * sizeof(char));
	dirRoot = (char*)malloc(50 * sizeof(char));

	// default values of root & port
	strcpy(dirRoot, "/home/user/res");
	strcpy(port,"10101"); // port=10101
	
	clients = (int*)malloc(CONEXMAX * sizeof(int));

	// server

	appName = argv[0];

	// process all command line arguments
	while ((value = getopt_long(argc, argv, "c:l:t:p:dh", long_options, &optionIndex)) != -1) {
		switch (value) {
			case 'c':
				confFileName = strdup(optarg);
				break;
			case 'l':
				logFileName = strdup(optarg);
				break;
			case 'p':
				pidFileName = strdup(optarg);
				break;
			case 't':
				return testConfFile(optarg);
			case 'd':
				startDaemonized = 1;
				break;
			case 'h':
				printHelp();
				return EXIT_SUCCESS;
			case '?':
				printHelp();
				return EXIT_FAILURE;
			default:
				break;
		}
	}

	// when daemonizing is requested at command line
	if (startDaemonized == 1) {
		daemonize();
	}

	// open system log and write message to it
	openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Started %s", appName);

	// Daemon will handle two signals
	signal(SIGINT, handleSignal);
	signal(SIGHUP, handleSignal);
	signal(SIGPIPE, SIG_IGN);
	
	// reads configuration from config file
	readConfFile(0);

	// scheduler as lottery
#ifdef MYPTHREAD
	pthread_setsched(RT);
#endif

	// try to open log file to this daemon
	if (logFileName[0] != '\0') {
		logStream = fopen(logFileName, "a+");
		if (logStream == NULL) {
			syslog(LOG_ERR, "Can not open log file: %s, error: %s",
				logFileName, strerror(errno));
			logStream = stdout;
			customLog=0;
			syslog(LOG_INFO, "Default Log file loaded @ /var/log/syslog");
		} else{
			customLog=1;
			syslog(LOG_INFO, "Log file loaded @ %s", logFileName);
		}
	} else {
		logStream = stdout;
		customLog=0;
		syslog(LOG_INFO, "Default Log file loaded @ /var/log/syslog");
	}

	// Thread structure to handle pre-thread function
	thr_ctl = calloc(workersNumber, sizeof(THREAD));
	if (!thr_ctl){
		fprintf(logStream,"%s > Error allocating thread control structure \n", getTime());;
		fflush(logStream);
	} else {
		fprintf(logStream,"%s > Thread control structure allocated for %d threads \n", getTime(), workersNumber);
		fflush(logStream);
	}

	// server

	fprintf(logStream,"%s > Server started @ Port: %s | Root directory: %s | Workers: %d\n",
			getTime(), port, dirRoot, workersNumber);
	fflush(logStream);

	if(customLog==1){
		fprintf(logStream,"%sLog file loaded @ %s", getTime(), appName);
		fflush(logStream);
	}

	int i;
	for (i=0; i<CONEXMAX; i++) {
		clients[i]=-1;

	}

	startServer(port);

	// Pre-threading Procedure
	for (int x = 0; x < workersNumber; x++){
		if((pthread_create(&thr_ctl[x].tid, NULL, requestResponse,(void *)&x)) != 0){
			fprintf(logStream,"%s > Thread create error \n", getTime());
			fflush(logStream);
		}
	}

	iget = iput = 0;
	int newSock;

	for(;;){
		addrLen = sizeof(clienteAddr);
		newSock = accept (sockfd, (struct sockaddr *) &clienteAddr, &addrLen);
		pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = newSock;
		if (++iput == MAXNCLI){
			iput = 0;
		}
		if (iput == iget){
			fprintf(logStream,"%s > iput = iget = %d \n", getTime(), iput);
			fflush(logStream);
		}
		pthread_cond_signal(&clifd_cond);
		pthread_mutex_unlock(&clifd_mutex);
	}
	
	// close log file, when it is used.
	if (logStream != stdout) {
		fclose(logStream);
	}


	// write system log and close it
	syslog(LOG_INFO, "Stopped %s", appName);
	closelog();

	// free allocated memory
	if (confFileName != NULL) free(confFileName);
	if (logFileName != NULL) free(logFileName);
	if (pidFileName != NULL) free(pidFileName);
	if (clients != NULL) free(clients);
	if (port != NULL) free(port);
	if (dirRoot != NULL) free(dirRoot);


	return EXIT_SUCCESS;
}
