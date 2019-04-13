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

<<<<<<< HEAD
	// Daemon will handle two signals
	signal(SIGINT, handleSignal);
	signal(SIGHUP, handleSignal);
	signal(SIGPIPE, SIG_IGN);
=======
	// Daemon will handle two signals plus one
	signal(SIGINT, handleSignal);
	signal(SIGHUP, handleSignal);
	sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
>>>>>>> aa5de01d94fc24963cec1265cdd75bb07a6e245a

	// reads configuration from config file
	readConfFile(0);

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

	// server

	fprintf(logStream,"%s > Server started @ Port: %s | Root directory: %s \n",
			getTime(), port, dirRoot);
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

	// this global variable can be changed in function handling signal
	running = 1;
	pthread_t threadRequest;
	int newSock;

	while (running == 1) {

		addrLen = sizeof(clienteAddr);
		// system call to create new socket connection
		newSock = accept (sockfd, (struct sockaddr *) &clienteAddr, &addrLen);
		fcntl(newSock, F_SETFL, O_NONBLOCK); // non-blocking socket

		if(newSock>0){
			struct args *SLOT = (struct args *)malloc(sizeof(struct args));
			SLOT->sslot=newSock;
			pthread_create(&threadRequest, NULL, requestResponse, (void *)SLOT);

		}
	}

	// server
	
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
