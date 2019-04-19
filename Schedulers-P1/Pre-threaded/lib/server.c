/*server.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernández A. (DaniHdez)

		Programming Language: C
		Version: 1.0
		Last Update: 05/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

static int outputPHP(const void*, unsigned int, void*);

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

		//int flags = fcntl(sockfd, F_GETFL);
		//fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); // to make it non-blocking

		// sets the socket addres
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) { 
			break;

		}
		if (listen(sockfd, CONEXMAX) < 0) {
		    fprintf(logStream,"%s > listen() or bind()\n", getTime());
		    fflush(logStream);
		}

	}
	if (p==NULL) { // case connection cannot be done
		fprintf(logStream,"%s > socket() or bind()\n", getTime());
		fflush(logStream);
		exit(1);

	}

	freeaddrinfo(res); // release mem to avoit memory leaks

	// listening new connections
	if ( listen (sockfd, MAX_QUEUE) != 0 ) {
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
	addrLen = sizeof(clienteAddr);
	int connfd;

	for (;;){
		//int socket = ((struct args*)input)->sslot;
		// system call to create new socket connection
		pthread_mutex_lock(&clifd_mutex);

		while (iget == iput){
			pthread_cond_wait(&clifd_cond, &clifd_mutex);
		}

		connfd = clifd[iget];   /* connected socket to service */

		if (++iget == MAXNCLI){
			iget = 0;
		}

		pthread_mutex_unlock(&clifd_mutex);
		thr_ctl[*((int *)input)].t_count++;







		//newSock = accept (sockfd, (struct sockaddr *) &clienteAddr, &addrLen);
		//fcntl(newSock, F_SETFL, O_NONBLOCK); // non-blocking socket
		pthread_mutex_unlock(&clifd_mutex);

		thr_ctl[*((int *)input)].t_count++;

		fprintf(logStream,"%s > ** Start communication with %i **\n", getTime(), connfd);
		fflush(logStream);

		char* reqline[3];
		char* data_to_send = (char*)malloc(BYTES * sizeof(char));
		char* message = (char*)malloc(MSGLEN * sizeof(char));
		char* path = (char*)malloc(MSGLEN * sizeof(char));

		int rcvd, fd, bytesLeidos;
		memset( (void*) message, (int)'\0', MSGLEN );

		while( (rcvd=recv(connfd, message, MSGLEN, 0)) <= 0 ){} // waits request from client

		if( (strcmp(message, "\n")) != 0){    // message received
			fprintf(logStream,"%s > Message received: \n\n%s", getTime(), message);
			fflush(logStream);
			reqline[0] = strtok (message, " \t\n");

			if ( strncmp(reqline[0], "GET\0", 4)==0 ){
				reqline[1] = strtok (NULL, " \t");
				reqline[2] = strtok (NULL, " \t\n");
				if ( strncmp( reqline[2], "HTTP/1.1", 8)!=0 )	{
					write(connfd, "HTTP/1.1 400 Bad Request\n", 25);

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
							#ifdef MYPTHREAD
								pthread_setpriority(lSize);
							#endif
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
								send(connfd, "HTTP/1.1 200 OK\n\n", 17, 0);
								while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
									write (connfd, data_to_send, bytesLeidos);
								}

							} else { // file not found
								write(connfd, "HTTP/1.1 404 Not Found\n", 23);

							}
							
						} else{

							#ifdef MYPTHREAD
								// Get file size to obtain priority (for LOTTERY schedule)
								size_t currentPos = lseek(fd, (size_t)0, SEEK_CUR);
								off_t fsize;
								fsize = lseek(fd, 0, SEEK_END);
								long size = (long) fsize;
								pthread_setpriority(size);

								if(isMultimedia(reqline[1])){ // to improve multimedia transfer
									pthread_setdeadline(size);

								}

								lseek(fd, currentPos, SEEK_SET);
							#endif

							send(connfd, "HTTP/1.1 200 OK\n\n", 17, 0);
							while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
								// spin to ensure the data was wrote correctly
								while( write(connfd, data_to_send, bytesLeidos)== -1){}
							}
						}

					}
					else { // file not found
						write(connfd, "HTTP/1.1 404 Not Found\n", 23);

					}
				}
			}
		}

		// closing socket
		close(connfd);
		close(fd);
		// release memory
		if(data_to_send!=NULL) free(data_to_send);
		if(message!=NULL) free(message);
		if(path!=NULL) free(path);
		fprintf(logStream,"%s > ** End communication with %i **\n", getTime(), connfd);
		fflush(logStream);
	}
	//return 0;
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
  *This function validates the value of scheduler
*/
int validateSCH(char * schName){
	if(strcmp(schName,"SRR")==0){
		return 0;
	}
	else if(strcmp(schName, "LOTTERY")==0){
		return 1;
	}
	else if(strcmp(schName, "RT")==0){
		return 2;
	}
	else if(strcmp(schName, "RR")==0){
		return 3;
	}
	else{
		printf("WARNING: Using default SRR scheduler, invalid parameter found %s\n", schName);
		return 0;
	}
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
	    else if (strcmp(parameter, "SCH")==0){
	    	strncpy (schedulerTmp, value, MAXLEN);
	    	ret += 1;
	    }
	    //If config file contains more information 
	    else{
	      syslog(LOG_INFO, "%s/%s: Unknown name/value pair!\n", parameter, value);
	      ret += 1;
	      
	    }
	}
	scheduler = malloc(strlen(schedulerTmp)+1);
	//Validate if the parameters were found
	if(strcmp(logFileTmp,"")==0 && strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
		return EXIT_FAILURE;
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    strcpy(scheduler, schedulerTmp);
	    schedulerID = validateSCH(scheduler);
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	}
	else{
		if(strcmp(logFileTmp,"")>0){
			if (strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(logFileName, logFileTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(schedulerTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(port, portTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(logFileName, logFileTmp);
	    		strcpy(port, portTmp);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(portTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
			}
			else{
				strcpy(logFileName, logFileTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
			}
		}
		else if(strcmp(portTmp,"")>0){
			if (strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(port, portTmp);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")>0){
				strcpy(port,portTmp);
				strcpy(dirRoot, rootTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")==0){
				strcpy(port, portTmp);
				strcpy(dirRoot, rootTmp);
			}
			else{
				strcpy(port,portTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);	
			}
		}
		else if (strcmp(rootTmp,"")>0){
			if(strcmp(schedulerTmp,"")==0){
				strcpy(dirRoot, rootTmp);
			}
			else{
				strcpy(dirRoot, rootTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
			}
		}
		else{
			strcpy(scheduler, schedulerTmp);
			schedulerID = validateSCH(scheduler);
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
	    else if (strcmp(parameter, "SCH")==0){
	    	strncpy (schedulerTmp, value, MAXLEN);
	    	ret += 1;
	    }
	    //If config file contains more information 
	    else{
	      printf ("WARNING: %s/%s: Unknown name/value pair!\n", parameter, value);
	      ret += 1;
	    }
	}
	scheduler = malloc(strlen(schedulerTmp)+1);
	//Validate if the parameters were found
	if(strcmp(logFileTmp,"")==0 && strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
		return EXIT_FAILURE;
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    strcpy(scheduler, schedulerTmp);
	    schedulerID = validateSCH(scheduler);
	    printf("Found LOGFILE:%s, PORT:%s , ROOT:%s and SCH:%s with code %d\n",logFileTmp,port,dirRoot,schedulerTmp, schedulerID);
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    printf("Found LOGFILE:%s, PORT:%s and ROOT:%s\n",logFileTmp,port,dirRoot);
	}
	else{
		if(strcmp(logFileTmp,"")>0){
			if (strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				printf("Just LOGFILE:%s found\n",logFileTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(schedulerTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(port, portTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
				printf("Found LOGFILE:%s, PORT:%s and SCH:%s with code %d\n", logFileTmp, port, scheduler, schedulerID);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
				printf("Found LOGFILE:%s, ROOT:%s and SCH:%s with code %d\n", logFileTmp, dirRoot, scheduler, schedulerID);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(logFileName, logFileTmp);
	    		strcpy(port, portTmp);
				printf("Found LOGFILE:%s and PORT:%s\n",logFileTmp,port);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(portTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
				printf("Found LOGFILE:%s and ROOT:%s\n",logFileTmp,dirRoot);
			}
			else{
				strcpy(logFileName, logFileTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
				printf("Found LOGFILE:%s and SCH:%s with code %d\n", logFileTmp, scheduler, schedulerID);
			}
		}
		else if(strcmp(portTmp,"")>0){
			if (strcmp(rootTmp,"")==0 && strcmp(schedulerTmp,"")==0){
				strcpy(port, portTmp);
				printf("Just PORT:%s found\n",port);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")>0){
				strcpy(port,portTmp);
				strcpy(dirRoot, rootTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
				printf("Found PORT:%s, ROOT:%s and SCH:%s with code %d\n",port, dirRoot, scheduler, schedulerID);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(schedulerTmp,"")==0){
				strcpy(port, portTmp);
				strcpy(dirRoot, rootTmp);
				printf("Found PORT:%s and ROOT:%s\n",port,dirRoot);
			}
			else{
				strcpy(port,portTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
				printf("Found PORT:%s and SCH:%s with code %d\n",port, scheduler, schedulerID);	
			}
		}
		else if (strcmp(rootTmp,"")>0){
			if(strcmp(schedulerTmp,"")==0){
				strcpy(dirRoot, rootTmp);
				printf("Just ROOT:%s found\n",dirRoot);
			}
			else{
				strcpy(dirRoot, rootTmp);
				strcpy(scheduler, schedulerTmp);
				schedulerID = validateSCH(scheduler);
				printf("Found ROOT:%s and SCH:%s with code %d\n", dirRoot, scheduler, schedulerID);
			}
		}
		else{
			strcpy(scheduler, schedulerTmp);
			schedulerID = validateSCH(scheduler);
			printf("Just SCH:%s found with code %d\n", scheduler, schedulerID);
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

	} else if(sig == SIGPIPE) {
		signal(SIGPIPE, SIG_DFL);
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

/**
 *	Verify if a request is a multimedia file
 */
int isMultimedia(char* request){
	int temp = 0;
	char* p = NULL;
	if((p = strstr(request, ".mp4")) != NULL 
	|| (p = strstr(request, ".wmv")) != NULL 
	|| (p = strstr(request, ".flv")) != NULL
	|| (p = strstr(request, ".avi")) != NULL){
		temp=1;
	}
    return temp;
}

void sig_int(int signo){
	int i;

	/* terminate all children */
	for (i = 0; i < numOfThreads; i++)
		pthread_detach(thr_ctl[i].tid);
		//kill(thr_ctl[i], SIGTERM);

	//while (wait(NULL) > 0);     /* wait for all children */
	
	if (errno != ECHILD)
		//err_sys("wait error");
	
	//pr_cpu_time();
	exit(0);
}

// void pr_cpu_time(void){
// 	double user, sys;
// 	struct rusage myusage, childusage;

// 	if (getrusage(RUSAGE_SELF, &myusage) < 0){
// 		fprintf(logStream,"%s > getrusage error \n", getTime());
// 		fflush(logStream);
// 	}
// 	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0){
// 		fprintf(logStream,"%s > getrusage error \n", getTime());
// 		fflush(logStream);
// 	}

// 	user = (double) myusage.ru_utime.tv_sec +
// 	myusage.ru_utime.tv_usec / 1000000.0;
// 	user += (double) childusage.ru_utime.tv_sec +
// 	childusage.ru_utime.tv_usec / 1000000.0;
// 	sys = (double) myusage.ru_stime.tv_sec +
// 	myusage.ru_stime.tv_usec / 1000000.0;
// 	sys += (double) childusage.ru_stime.tv_sec +
// 	childusage.ru_stime.tv_usec / 1000000.0;

// 	fprintf(logStream,"%s > user time = %g, sys time = %g\n", getTime(), user, sys);
// 	fflush(logStream);
// }

/*server.c*/
