/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 26/02/2019

		Inputs: Port and/ or root directory
		Restrictions: Numeric Value / String

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

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

	int value, option_index = 0;
	log_file_name = NULL;
	start_daemonized = 0;
	running = 0;
	conf_file_name = NULL;
	pid_file_name = NULL;
	pid_fd = -1;
	app_name = NULL;

	// server
	
	// default root = ~/res and port=10101
	char* port = (char*)malloc(6 * sizeof(char));
	//dirRoot = getenv("HOME"); // gets the current directory
	//strcat(dirRoot, "/res"); // $(HOME)/res
	dirRoot = "/home/estape11/res";
	strcpy(port,"10101"); // port=10101
	int slot=0;
	clients = (int*)malloc(CONEXMAX * sizeof(int));

	// server

	app_name = argv[0];

	/* Try to process all command line arguments */
	while ((value = getopt_long(argc, argv, "c:l:t:p:dh", long_options, &option_index)) != -1) {
		switch (value) {
			case 'c':
				conf_file_name = strdup(optarg);
				break;
			case 'l':
				log_file_name = strdup(optarg);
				break;
			case 'p':
				pid_file_name = strdup(optarg);
				break;
			case 't':
				return test_conf_file(optarg);
			case 'd':
				start_daemonized = 1;
				break;
			case 'h':
				print_help();
				return EXIT_SUCCESS;
			case '?':
				print_help();
				return EXIT_FAILURE;
			default:
				break;
		}
	}

	/* When daemonizing is requested at command line. */
	if (start_daemonized == 1) {
		/* It is also possible to use glibc function deamon()
		 * at this point, but it is useful to customize your daemon. */
		daemonize();
	}

	/* Open system log and write message to it */
	openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Started %s", app_name);

	/* Daemon will handle two signals */
	signal(SIGINT, handle_signal);
	signal(SIGHUP, handle_signal);

	/* Try to open log file to this daemon */
	if (log_file_name != NULL) {
		log_stream = fopen(log_file_name, "a+");
		if (log_stream == NULL) {
			syslog(LOG_ERR, "Can not open log file: %s, error: %s",
				log_file_name, strerror(errno));
			log_stream = stdout;
		}
	} else {
		log_stream = stdout;
	}

	/* Read configuration from config file */
	read_conf_file(0);

	// server

	fprintf(log_stream,"> Server started\n\t Port: %s%s%s \n\t Root directory: %s%s%s\n","\033[92m",
			port,"\033[0m","\033[92m",dirRoot,"\033[0m");
	fflush(log_stream);

	int i;
	for (i=0; i<CONEXMAX; i++) {
		clients[i]=-1;

	}

	startServer(port);

	/* This global variable can be changed in function handling signal */
	running = 1;

	/* Never ending loop of server */
	while (running == 1) {

		addrLen = sizeof(clienteAddr);
		// system call to create new socket connection
		clients[slot] = accept (sockfd, (struct sockaddr *) &clienteAddr, &addrLen);

		if (clients[slot]<0){
			fprintf(log_stream,"accept() error\n");
			fflush(log_stream);

		}

		else {
			requestResponse(slot); // serve one request at the time

		}

		while (clients[slot]!=-1) {
			slot = (slot+1)%CONEXMAX; 

		}

	}

	// server
	
	/* Close log file, when it is used. */
	if (log_stream != stdout) {
		fclose(log_stream);
	}

	/* Write system log and close it. */
	syslog(LOG_INFO, "Stopped %s", app_name);
	closelog();

	/* Free allocated memory */
	if (conf_file_name != NULL) free(conf_file_name);
	if (log_file_name != NULL) free(log_file_name);
	if (pid_file_name != NULL) free(pid_file_name);

	return EXIT_SUCCESS;
}