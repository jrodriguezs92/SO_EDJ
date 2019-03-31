#include <server.h>

int main(int argc, char* argv[])
{

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

	int slot=0;

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

	fprintf(logStream,"%s > Server started @ Port: %s | Root directory: %s \n",
			getTime(), port, dirRoot);
	fflush(logStream);

	if(customLog==1){
		fprintf(logStream,"%sLog file loaded @ %s", getTime(), appName);
		fflush(logStream);
	}
	
	// Setting all elements to -1: signifies there is no client connected
	int i;
	for (i=0; i<CONNMAX; i++)
		clients[i]=-1;
	startServer(port);

	// this global variable can be changed in function handling signal
	running = 1;

	// ACCEPT connections
	while (running==1) {

		addrlen = sizeof(clienteAddr);
		clients[slot] = accept (listenfd, (struct sockaddr *) &clienteAddr, &addrlen);

		if (clients[slot]<0) {
			fprintf(logStream,"%s > accept() error\n", getTime());
			fflush(logStream);
		}
		else
		{
			if ( fork()==0 )
			{
				respond(slot);
				exit(0);
			}
		}

		while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
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

	return EXIT_SUCCESS;;
}