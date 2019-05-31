/*server.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 28/02/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

/**
 * this function starts the server @ port
 */
void startServer(char* port) {
	struct addrinfo hints, *res, *p;

	// getaddrinfo for the host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo( NULL, port, &hints, &res) != 0) {
		fprintf(logStream,"%s > getaddrinfo() error\n", getTime());
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
		    fprintf(logStream,"%s > listen() or bind()\n", getTime());
		    fflush(logStream);
		}

	}
	if (p==NULL) { // case connection can be done
		fprintf(logStream,"%s > socket() or bind()\n", getTime());
		fflush(logStream);
		exit(1);

	}

	freeaddrinfo(res); // release mem to avoit memory leaks

	// listening new connections
	if ( listen (sockfd, 1000000) != 0 ) {
		fprintf(logStream,"%s > listen() error\n", getTime());
		fflush(logStream);
		exit(1);

	}

}

/**
 * this funtion response a client request
 */
void requestResponse(int n) {
	fprintf(logStream,"%s > ** Start communication with %i **\n", getTime(), n);
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
		fprintf(logStream,"%s > recv() error\n", getTime());
		fflush(logStream);

	}

	else if (rcvd==0) {    // socket closed
		fprintf(logStream,"%s > Client disconnected.\n", getTime());
		fflush(logStream);

	}

	else if((strcmp(message, "\n")) != 0){    // message received
		fprintf(logStream,"%s > Message received: \n\n%s", getTime(), message);
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
				fprintf(logStream,"%s > Sending: %s\n", getTime(), path);
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
	fprintf(logStream,"%s > ** End communication with %i **\n", getTime(), n);
	fflush(logStream);
}

/** 
  *This function trims the value found in readConfFile
 */
char * trimValue (char * line){
  //Initialize start, end pointers 
  char *start = line, *end = &line[strlen (line) - 1];

  //Trim right side 
  while ( (isspace (*end)) && (end >= start) )
    end--;
  *(end+1) = '\0';

  //Trim left side 
  while ( (isspace (*start)) && (start < end) )
    start++;

  //Copy finished string 
  strcpy (line, start);
  return line;
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

	// SETEAR: dirRoot | port | logFileName 

	char *line, buff[256];
	ret = 0;
	while ((line = fgets (buff, sizeof buff, conf_file)) != NULL){
	    //Ignore comments with "#" and blankspaces
	    if (buff[0] == '\n' || buff[0] == '#'){
	      continue;
	    }
	    
	    //Get words after and before "="
	    char parameter[MAXLEN], value[MAXLEN];
	    line = strtok (buff, "=");
	    if (line==NULL)
	      continue;
	    else {
	      strncpy (parameter, line, MAXLEN);
	      line = strtok (NULL, "=");
	    }
	    if (line==NULL)
	      continue;
	    else
	    {
	      strncpy (value, line, MAXLEN);
	      trimValue (value);
	    }
	    
	    //Get the value of the parameters
	    if (strcmp(parameter, "LOGFILE")==0){
	      strncpy (logFileTmp, value, MAXLEN);
	      ret += 1;
	    }
	    else if (strcmp(parameter, "PORT")==0){
	      strncpy (portTmp, value, MAXLEN);
	      ret += 1;
	    }
	    else if (strcmp(parameter, "ROOT")==0){
	      strncpy (rootTmp, value, MAXLEN);
	      ret += 1;
	    }
	    //If config file contains more information 
	    else{
	      syslog(LOG_INFO, "%s/%s: Unknown name/value pair!\n", parameter, value);
	      ret += 1;
	      
	    }
	}
	
	//Validate if the 3 parameters were found
	if(strcmp(logFileTmp,"")==0 && strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0){
		return EXIT_FAILURE;
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	}
	else{
		if(strcmp(logFileTmp,"")>0){
			if (strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0){
				strcpy(logFileName, logFileTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(rootTmp,"")==0){
				strcpy(logFileName, logFileTmp);
	    		strcpy(port, portTmp);
			}
			else{
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
			}
		}
		else if(strcmp(portTmp,"")>0){
			if (strcmp(rootTmp,"")==0){
				strcpy(port, portTmp);
			}
			else{
				strcpy(port, portTmp);
				strcpy(dirRoot, rootTmp);
			}
		}
		else{
			strcpy(dirRoot, rootTmp);

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

	char *line, buff[256];
	ret = 0;
	while ((line = fgets (buff, sizeof buff, conf_file)) != NULL){
	    //Ignore comments with "#" and blankspaces
	    if (buff[0] == '\n' || buff[0] == '#'){
	      continue;
	    }
	    
	    //Get words after and before "="
	    char parameter[MAXLEN], value[MAXLEN];
	    line = strtok (buff, "=");
	    if (line==NULL)
	      continue;
	    else {
	      strncpy (parameter, line, MAXLEN);
	      line = strtok (NULL, "=");
	    }
	    if (line==NULL)
	      continue;
	    else
	    {
	      strncpy (value, line, MAXLEN);
	      trimValue (value);
	    }
	    
	    //Get the value of the parameters
	    if (strcmp(parameter, "LOGFILE")==0){
	      strncpy (logFileTmp, value, MAXLEN);
	      ret += 1;
	    }
	    else if (strcmp(parameter, "PORT")==0){
	      strncpy (portTmp, value, MAXLEN);
	      ret += 1;
	    }
	    else if (strcmp(parameter, "ROOT")==0){
	      strncpy (rootTmp, value, MAXLEN);
	      ret += 1;
	    }
	    //If config file contains more information 
	    else{
	      printf ("WARNING: %s/%s: Unknown name/value pair!\n", parameter, value);
	      ret += 1;
	    }
	}
	
	//Validate if the 3 parameters were found
	if(strcmp(logFileTmp,"")==0 && strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0){
		return EXIT_FAILURE;
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    printf("Found LOGFILE:%s, PORT:%s y ROOT:%s\n",logFileTmp,port,dirRoot);
	}
	else{
		if(strcmp(logFileTmp,"")>0){
			if (strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				printf("Just LOGFILE:%s found\n",logFileTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(rootTmp,"")==0){
				strcpy(logFileName, logFileTmp);
	    		strcpy(port, portTmp);
				printf("Found LOGFILE:%s, PORT:%s\n",logFileTmp,port);
			}
			else{
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
				printf("Found LOGFILE:%s y ROOT:%s\n",logFileTmp,dirRoot);
			}
		}
		else if(strcmp(portTmp,"")>0){
			if (strcmp(rootTmp,"")==0){
				strcpy(port, portTmp);
				printf("Just PORT:%s found\n",port);
			}
			else{
				strcpy(port, portTmp);
				strcpy(dirRoot, rootTmp);
				printf("Found PORT:%s y ROOT:%s\n",port,dirRoot);
			}
		}
		else{
			strcpy(dirRoot, rootTmp);
			printf("Just ROOT:%s found\n",dirRoot);
		}
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
		fprintf(logStream, "%s > Debug: stopping daemon ...\n", getTime());
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
		fprintf(logStream, "%s > Debug: reloading daemon config file ...\n", getTime());
		// falta implementar
		fflush(logStream);
		readConfFile(1);

	} else if (sig == SIGCHLD) {
		fprintf(logStream, "%s > Debug: received SIGCHLD signal\n", getTime());
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

/**
 *  gets the current date time
 */
char* getTime(void){
	char* timeString;
	if(customLog==1){
		time_t tm = time(NULL);
    	timeString = strtok(ctime(&tm), "\n"); // removes new line
	} else{
		timeString="";
	}
	return timeString;
}

/*server.c*/
