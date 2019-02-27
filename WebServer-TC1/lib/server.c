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
		fprintf(log_stream,"getaddrinfo() error\n");
		fflush(log_stream);
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
		    fprintf(log_stream,"listen() or bind()\n");
		    fflush(log_stream);
		}

	}
	if (p==NULL) { // case connection can be done
		//perror ("socket() or bind()");
		fprintf(log_stream,"socket() or bind()\n");
		fflush(log_stream);
		exit(1);

	}

	freeaddrinfo(res); // release mem to avoit memory leaks

	// listening new connections
	if ( listen (sockfd, 1000000) != 0 ) {
		//perror("listen() error");
		fprintf(log_stream,"listen() error\n");
		fflush(log_stream);
		exit(1);

	}

	//fcntl(sockfd, F_SETFL, O_NONBLOCK); // change the socket into non-blocking state

}

void requestResponse(int n) {
	fprintf(log_stream,"\n** Start communication with %i **\n",n);
	fflush(log_stream);
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
		fprintf(log_stream,("recv() error\n"));
		fflush(log_stream);

	}

	else if (rcvd==0) {    // socket closed
		fprintf(log_stream,"> Client disconnected.\n");
		fflush(log_stream);

	}

	else if((strcmp(message, "\n")) != 0){    // message received
		fprintf(log_stream,"Message received: \n%s", message);
		fflush(log_stream);
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
				fprintf(log_stream,"Sending: %s\n", path);
				fflush(log_stream);

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
	fprintf(log_stream,"\n** End communication with %i **\n",n);
	fflush(log_stream);
}

/**
 * \brief Read configuration from config file
 */
int read_conf_file(int reload)
{
	FILE *conf_file = NULL;
	int ret = -1;

	if (conf_file_name == NULL) return 0;

	conf_file = fopen(conf_file_name, "r");

	if (conf_file == NULL) {
		syslog(LOG_ERR, "Can not open config file: %s, error: %s",
				conf_file_name, strerror(errno));
		return -1;
	}

	ret = fscanf(conf_file, "%d", &delay);

	if (ret > 0) {
		if (reload == 1) {
			syslog(LOG_INFO, "Reloaded configuration file %s of %s",
				conf_file_name,
				app_name);
		} else {
			syslog(LOG_INFO, "Configuration of %s read from file %s",
				app_name,
				conf_file_name);
		}
	}

	fclose(conf_file);

	return ret;
}

/**
 * \brief This function tries to test config file
 */
int test_conf_file(char *_conf_file_name)
{
	FILE *conf_file = NULL;
	int ret = -1;

	conf_file = fopen(_conf_file_name, "r");

	if (conf_file == NULL) {
		fprintf(stderr, "Can't read config file %s\n",
			_conf_file_name);
		return EXIT_FAILURE;
	}

	ret = fscanf(conf_file, "%d", &delay);

	if (ret <= 0) {
		fprintf(stderr, "Wrong config file %s\n",
			_conf_file_name);
	}

	fclose(conf_file);

	if (ret > 0)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

/**
 * \brief Callback function for handling signals.
 * \param	sig	identifier of signal
 */
void handle_signal(int sig)
{
	if (sig == SIGINT) {
		fprintf(log_stream, "Debug: stopping daemon ...\n");
		fflush(log_stream);
		/* Unlock and close lockfile */
		if (pid_fd != -1) {
			lockf(pid_fd, F_ULOCK, 0);
			close(pid_fd);
		}
		/* Try to delete lockfile */
		if (pid_file_name != NULL) {
			unlink(pid_file_name);
		}
		running = 0;

		// server

		close(sockfd);

		// server

		/* Reset signal handling to default behavior */
		signal(SIGINT, SIG_DFL);
	} else if (sig == SIGHUP) {
		fprintf(log_stream, "Debug: reloading daemon config file ...\n");
		fflush(log_stream);
		read_conf_file(1);
	} else if (sig == SIGCHLD) {
		fprintf(log_stream, "Debug: received SIGCHLD signal\n");
		fflush(log_stream);
	}
}

/**
 * \brief This function will daemonize this app
 */
void daemonize()
{
	pid_t pid = 0;
	int fd;

	/* Fork off the parent process */
	pid = fork();

	/* An error occurred */
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Success: Let the parent terminate */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* On success: The child process becomes session leader */
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	/* Ignore signal sent from child to parent process */
	signal(SIGCHLD, SIG_IGN);

	/* Fork off for the second time*/
	pid = fork();

	/* An error occurred */
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Success: Let the parent terminate */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");

	/* Close all open file descriptors */
	for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
		close(fd);
	}

	/* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
	stdin = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");

	/* Try to write PID of daemon to lockfile */
	if (pid_file_name != NULL)
	{
		char str[256];
		pid_fd = open(pid_file_name, O_RDWR|O_CREAT, 0640);
		if (pid_fd < 0) {
			/* Can't open lockfile */
			exit(EXIT_FAILURE);
		}
		if (lockf(pid_fd, F_TLOCK, 0) < 0) {
			/* Can't lock file */
			exit(EXIT_FAILURE);
		}
		/* Get current PID */
		sprintf(str, "%d\n", getpid());
		/* Write PID to lockfile */
		write(pid_fd, str, strlen(str));
	}
}

/**
 * \brief Print help for this application
 */
void print_help(void)
{
	printf("\n Usage: %s [OPTIONS]\n\n", app_name);
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