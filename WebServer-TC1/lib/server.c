/*server.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 26/02/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

void startServer(char* puerto) {
	struct addrinfo hints, *res, *p;

	// getaddrinfo for the host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo( NULL, puerto, &hints, &res) != 0) {
		//perror ("getaddrinfo() error");
		fprintf(logStream,"getaddrinfo() error\n");
		fflush(logStream);
		exit(1);

	}
	int on = 1;

	// socket and bind
	for (p = res; p!=NULL; p=p->ai_next){
		// creates the connection point
		sockfd = socket (p->ai_family, p->ai_socktype, 0); 

		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		if (sockfd == -1) {
			continue;

		}

		// sets the socket addres
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) { 
			break;

		}
		if (listen(sockfd, CONEXMAX) < 0) {
		    fprintf(logStream,"listen() or bind()\n");
		    fflush(logStream);
		}

	}
	if (p==NULL) { // case connection can be done
		//perror ("socket() or bind()");
		fprintf(logStream,"socket() or bind()\n");
		fflush(logStream);
		exit(1);

	}

	freeaddrinfo(res); // release mem to avoit memory leaks

	// listening new connections
	if ( listen (sockfd, 1000000) != 0 ) {
		//perror("listen() error");
		fprintf(logStream,"listen() error\n");
		fflush(logStream);
		exit(1);

	}

	//fcntl(sockfd, F_SETFL, O_NONBLOCK); // change the socket into non-blocking state

}

void requestResponse(int n) {
	fprintf(logStream,"\n** Start communication with %i **\n",n);
	fflush(logStream);
	char* reqline[3];
	char* data_to_send = (char*)malloc(BYTES * sizeof(char));
	char* message = (char*)malloc(MSGLEN * sizeof(char));
	char* path = (char*)malloc(MSGLEN * sizeof(char));
	
	int rcvd, fd, bytesLeidos;
	memset( (void*) message, (int)'\0', MSGLEN );

	// 5s timeout
	struct timeval tv = {5, 0};
	setsockopt(clients[n], SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	rcvd=recv(clients[n], message, MSGLEN, 0);

	if (rcvd<0) {    // receive an error
		fprintf(logStream,("recv() error\n"));
		fflush(logStream);

	}

	else if (rcvd==0) {    // socket closed
		fprintf(logStream,"> Client disconnected.\n");
		fflush(logStream);

	}

	else if((strcmp(message, "\n")) != 0){    // message received
		fprintf(logStream,"Message received: \n%s", message);
		fflush(logStream);
		reqline[0] = strtok (message, " \t\n");

		if ( strncmp(reqline[0], "GET\0", 4)==0 ){
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.1", 8)!=0 )	{
				write(clients[n], "HTTP/1.1 400 Bad Request\n", 25);

			}

			else {
				if ( strncmp(reqline[1], "/\0", 2)==0 ) {
					reqline[1] = "/index.html"; // default file to show
				}

				strcpy(path, dirRoot);
				strcpy(&path[strlen(dirRoot)], reqline[1]);
				fprintf(logStream,"Sending: %s\n", path);
				fflush(logStream);

				if ( (fd=open(path, O_RDONLY))!=-1 ) { // file found
					send(clients[n], "HTTP/1.1 200 OK\n\n", 17, 0);

					while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
						write (clients[n], data_to_send, bytesLeidos);

					}

				}
				else { // file not found
					write(clients[n], "HTTP/1.1 404 Not Found\n", 23);

				}
			}
		}
	}

	// closing socket
	close(clients[n]);
	clients[n]=-1;
	fprintf(logStream,"\n** End communication with %i **\n",n);
	fflush(logStream);
}

/**
 * this function read configuration from config file
 */
int readConfFile(int reload){
	FILE *conf_file = NULL;
	int ret = -1;

	if (confFileName == NULL) {
		return 0;

	}

	conf_file = fopen(confFileName, "r");

	if (conf_file == NULL) {
		syslog(LOG_ERR, "Can not open config file: %s, error: %s",
				confFileName, strerror(errno));
		return -1;

	}

	ret = fscanf(conf_file, "%d", &delay);

	if (ret > 0) {
		if (reload == 1) {
			syslog(LOG_INFO, "Reloaded configuration file %s of %s",
				confFileName,
				appName);

		} else {
			syslog(LOG_INFO, "Configuration of %s read from file %s",
				appName,
				confFileName);

		}
	}

	fclose(conf_file);

	return ret;
}

/**
 * this function tries to test config file
 */
int testConfFile(char *_confFileName){
	FILE *conf_file = NULL;
	int ret = -1;

	conf_file = fopen(_confFileName, "r");

	if (conf_file == NULL) {
		fprintf(stderr, "Can't read config file %s\n",
			_confFileName);
		return EXIT_FAILURE;

	}

	ret = fscanf(conf_file, "%d", &delay);

	if (ret <= 0) {
		fprintf(stderr, "Wrong config file %s\n",
			_confFileName);

	}

	fclose(conf_file);

	if (ret > 0){
		return EXIT_SUCCESS;

	}
	else{
		return EXIT_FAILURE;

	}
}

/**
 * callback function for handling signals
 */
void handleSignal(int sig){
	if (sig == SIGINT) {
		fprintf(logStream, "Debug: stopping daemon ...\n");
		fflush(logStream);

		// unlock and close lockfile
		if (pidFd != -1) {
			lockf(pidFd, F_ULOCK, 0);
			close(pidFd);

		}
		// try to delete lockfile
		if (pidFileName != NULL) {
			unlink(pidFileName);

		}

		running = 0;

		// server

		close(sockfd);

		// server

		// reset signal handling to default behavior
		signal(SIGINT, SIG_DFL);

	} else if (sig == SIGHUP) {
		fprintf(logStream, "Debug: reloading daemon config file ...\n");
		fflush(logStream);
		readConfFile(1);

	} else if (sig == SIGCHLD) {
		fprintf(logStream, "Debug: received SIGCHLD signal\n");
		fflush(logStream);

	}
}

/**
 * this function will daemonize the server
 */
void daemonize(){
	pid_t pid = 0;
	int fd;

	// fork off the parent process
	pid = fork();

	// an error occurred
	if (pid < 0) {
		exit(EXIT_FAILURE);

	}

	// success: let the parent terminate
	if (pid > 0) {
		exit(EXIT_SUCCESS);

	}

	// on success: the child process becomes session leader
	if (setsid() < 0) {
		exit(EXIT_FAILURE);

	}

	// ignore signal sent from child to parent process
	signal(SIGCHLD, SIG_IGN);

	// fork off for the second time
	pid = fork();

	// an error occurred
	if (pid < 0) {
		exit(EXIT_FAILURE);

	}

	// success: let the parent terminate
	if (pid > 0) {
		exit(EXIT_SUCCESS);

	}

	// set new file permissions
	umask(0);

	// Change the working directory to the root directory or another appropriated directory
	chdir("/");

	// Close all open file descriptors 
	for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
		close(fd);
	}

	// reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2)
	stdin = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");

	// try to write PID of daemon to lockfile
	if (pidFileName != NULL) {
		char str[256];
		pidFd = open(pidFileName, O_RDWR|O_CREAT, 0640);

		if (pidFd < 0) {
			// can't open lockfile
			exit(EXIT_FAILURE);

		}
		if (lockf(pidFd, F_TLOCK, 0) < 0) {
			// can't lock file
			exit(EXIT_FAILURE);

		}

		// get current PID
		sprintf(str, "%d\n", getpid());

		// write PID to lockfile
		write(pidFd, str, strlen(str));

	}
}

/**
 * prints help for this application
 */
void printHelp(void){
	printf("\n Usage: %s [OPTIONS]\n\n", appName);
	printf("  Options:\n");
	printf("   -h --help                 Print this help\n");
	printf("   -c --conf_file filename   Read configuration from the file\n");
	printf("   -t --test_conf filename   Test configuration file\n");
	printf("   -l --log_file  filename   Write logs to the file\n");
	printf("   -d --daemon               Daemonize this application\n");
	printf("   -p --pid_file  filename   PID file used by daemonized app\n");
	printf("\n");

}

/*server.c*/