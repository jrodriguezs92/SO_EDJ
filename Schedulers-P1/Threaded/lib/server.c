/*server.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernández A. (DaniHdez)

		Programming Language: C
		Version: 1.0
		Last Update: 03/04/2019

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
//void requestResponse(int n) {
void *requestResponse(void * input){
	int n = ((struct args*)input)->sslot;

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
					if(isPHPRequest(reqline[1])){ // if php
						close(fd); // its only used to test if the file exists
						ph7 *pEngine; /* PH7 engine */
						ph7_vm *pVm;  /* Compiled PHP program */
						int rc;
						/* Allocate a new PH7 engine instance */
						rc = ph7_init(&pEngine);

						if( rc != PH7_OK ){
							/*
							 * If the supplied memory subsystem is so sick that we are unable
							 * to allocate a tiny chunk of memory, there is no much we can do here.
							 */
							fatalError("Error while allocating a new PH7 engine instance");
						}


						FILE* fdTemp = fopen(path, "r");
						long lSize;

						fseek( fdTemp , 0L , SEEK_END); //use the function instead
						lSize = ftell( fdTemp );       // to know the file size
						rewind( fdTemp );             // Now point to beginning 

						char* speech = calloc( 1, lSize+1 );
						if( speech )
						{
						    if( fread( speech , lSize, 1 , fdTemp) != 1)
						    {
						      fclose(fdTemp) ;
						      free(speech); 
						      //exit(1);
						    }
						}

						fclose(fdTemp); 
						

						/* Compile the PHP test program defined above */
						rc = ph7_compile_v2(
							pEngine,  /* PH7 engine */
							speech, /* PHP test program */
							-1        /* Compute input length automatically*/, 
							&pVm,     /* OUT: Compiled PHP program */
							0         /* IN: Compile flags */
							);

						if( rc != PH7_OK ){
							if( rc == PH7_COMPILE_ERR ){
								const char *zErrLog;
								int nLen;
								/* Extract error log */
								ph7_config(pEngine, 
									PH7_CONFIG_ERR_LOG, 
									&zErrLog, 
									&nLen
									);
								if( nLen > 0 ){
									/* zErrLog is null terminated */
									puts(zErrLog);
								}
							}
							/* Exit */
							fatalError("Compile error");
						}
						strcpy(&path[strlen(dirRoot)+strlen(reqline[1])], ".html");
						file = fopen(path, "w");

						/*
						 * Now we have our script compiled, it's time to configure our VM.
						 * We will install the VM output consumer callback defined above
						 * so that we can consume the VM output and redirect it to STDOUT.
						 */
						rc = ph7_vm_config(pVm, 
							PH7_VM_CONFIG_OUTPUT, 
							outputPHP,    /* Output Consumer callback */
							0                   /* Callback private data */
							);
						if( rc != PH7_OK ){
							fatalError("Error while installing the VM output consumer callback");
						}
						/*
						 * And finally, execute our program. Note that your output (STDOUT in our case)
						 * should display the result.
						 */
						ph7_vm_exec(pVm, 0);
						/* All done, cleanup the mess left behind.
						*/
						ph7_vm_release(pVm);
						ph7_release(pEngine);
						free(speech); // Don't forget to free the allocated memory !
						
						fclose(file);

						if ( (fd=open(path, O_RDONLY))!=-1 ) { // file found
							send(clients[n], "HTTP/1.1 200 OK\n\n", 17, 0);

							while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
								write (clients[n], data_to_send, bytesLeidos);
							}
						} else { // file not found
							write(clients[n], "HTTP/1.1 404 Not Found\n", 23);

						}
						
					} else{
						send(clients[n], "HTTP/1.1 200 OK\n\n", 17, 0);

						while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
							write (clients[n], data_to_send, bytesLeidos);

						}
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
	close(fd);
	//close(fd);
	//if(fdPHP!=NULL){
	//	fclose(fdPHP);
	//}
	//if(path!=NULL){
	//	free(path);
	//}
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

/* 
 * Display an error (php interpreter) message and exit.
 */
void fatalError(const char *zMsg) {
	puts(zMsg);
	/* Shutdown the library */
	ph7_lib_shutdown();
	/* Exit immediately */
	exit(0);
}

/*
 * VM output consumer callback.
 * Each time the virtual machine generates some outputs, the following
 * function gets called by the underlying virtual machine  to consume
 * the generated output.
 * All this function does is redirecting the VM output to STDOUT.
 * This function is registered later via a call to ph7_vm_config()
 * with a configuration verb set to: PH7_VM_CONFIG_OUTPUT.
 */
static int outputPHP(const void *pOutput, unsigned int nOutputLen, void *pUserData /* Unused */) {
	fprintf (file, "%.*s", nOutputLen, (const char *)pOutput); // file is temp_.html
	return PH7_OK;
}

/**
 *	Verify if a request is a php file
 */
int isPHPRequest(char* request){
	int temp = 0;
	char* p = NULL;
	p = strstr(request, ".php");
	if(p!=NULL){
		temp=1;
	}
    return temp;
}
/*server.c*/
