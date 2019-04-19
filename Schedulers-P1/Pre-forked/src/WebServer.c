/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)

		Programming Language: C
		Version: 1.0
		Last Update: 18/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

int main(int argc, char* argv[]) {	
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
	running = false;
	confFileName = NULL;
	pidFileName = NULL;
	pidFd = -1;
	appName = NULL;
	workersNumber = 1; // default workers

	// server
	
	port = (char*)malloc(10 * sizeof(char));
	dirRoot = (char*)malloc(50 * sizeof(char));

	// default values of root & port
	strcpy(dirRoot, "/home/user/res");
	strcpy(port,"10101"); // port=10101

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
	signal(SIGCHLD, handleSignal);

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

	fprintf(logStream,"%s > Server started @ Port: %s | Root directory: %s | Workers: %d\n",
			getTime(), port, dirRoot, workersNumber);
	fflush(logStream);

	if (customLog==1){
		fprintf(logStream,"%sLog file loaded @ %s", getTime(), appName);
		fflush(logStream);
	}

	startServer(port);

	// This global variable can be changed in function handling signal
	running = true;
	int lastSocket, i;
	bool foundWorker;

	// Initialize the workers
	workers = malloc(sizeof(WCB)*workersNumber);
	for (i=0; i<workersNumber ; i++){
		workers[i] = malloc(sizeof(struct wcb)); // initialize the worker
		workers[i]->busy = false;
		workers[i]->socket = -1;

		if ( forkWorker(workers[i]) == -1) {
			fprintf(logStream,"%s > clone() error\n", getTime());
			fflush(logStream);
			exit(1);

		}
		
	}

	// ACCEPT connections
	while( running ) {
		addrlen = sizeof(clienteAddr);
		lastSocket = accept (listenfd, (struct sockaddr *) &clienteAddr, &addrlen);

		if (lastSocket<0) {
			fprintf(logStream,"%s > accept() error\n", getTime());
			fflush(logStream);

		} else {
			foundWorker = false;
			while(!foundWorker){
				for(i=0;i<workersNumber;i++){
					if(!workers[i]->busy){
						workers[i]->socket = lastSocket;
						workers[i]->busy = true;
						foundWorker = true;
						break;

					} // not found yet
				}

			} // do until found some worker free			
		}
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
	if (port != NULL) free(port);
	if (dirRoot != NULL) free(dirRoot);
	for(i=0;i<workersNumber;i++){free(workers[i]);}
	free(workers);

	return EXIT_SUCCESS;;
}