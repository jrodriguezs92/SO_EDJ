/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernández A (DaniHdez)

		Programming Language: C
		Version: 1.0
		Last Update: 18/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

// Static prototypes
static int outputPHP(const void *, unsigned int, void *);
static int childProcessHandler(void*);
static void respond(int);

//start server
void startServer(char *port)
{
	struct addrinfo hints, *res, *p;

	// getaddrinfo for host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo( NULL, port, &hints, &res) != 0)
	{
		fprintf(logStream,"%s > getaddrinfo() error\n", getTime());
		fflush(logStream);
		exit(1);
	}
	// socket and bind
	int on = 1;
	for (p = res; p!=NULL; p=p->ai_next)
	{
		listenfd = socket (p->ai_family, p->ai_socktype, 0);
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (listenfd == -1) continue;
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
		if (listen(listenfd, CONNMAX) < 0) {
		    fprintf(logStream,"%s > listen() or bind()\n", getTime());
		    fflush(logStream);
		}
	}
	if (p==NULL)
	{
		fprintf(logStream,"%s > socket() or bind()\n", getTime());
		fflush(logStream);
		perror ("socket() or bind()");
		exit(1);
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen (listenfd, CONNMAX) != 0 )
	{
		fprintf(logStream,"%s > listen() error\n", getTime());
		fflush(logStream);
		exit(1);
	}
}

//client connection
static void respond(int socket) {

	fprintf(logStream,"%s > ** Start communication with %i **\n", getTime(), socket);
	fflush(logStream);
	char mesg[MSGLEN], *reqline[3], data_to_send[BYTES], path[MSGLEN];
	int rcvd, fd, bytes_read;

	memset( (void*)mesg, (int)'\0', MSGLEN );

	rcvd=recv(socket, mesg, MSGLEN, 0);

	if (rcvd<0) {    // receive error
		fprintf(logStream,"%s > recv() error\n", getTime());
		fflush(logStream);
	}
	else if (rcvd==0) {   // receive socket closed
		fprintf(logStream,"%s > Client disconnected.\n", getTime());
		fflush(logStream);
	}
	else    // message received
	{
		fprintf(logStream,"%s > Message received: \n\n%s", getTime(), mesg);
		fflush(logStream);

		reqline[0] = strtok (mesg, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				write(socket, "HTTP/1.0 400 Bad Request\n", 25);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
					reqline[1] = "/index.html"; //default file to open

				strcpy(path, dirRoot);
				strcpy(&path[strlen(dirRoot)], reqline[1]);
				fprintf(logStream,"%s > Sending: %s\n", getTime(), path);
				fflush(logStream);

				if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
				{
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

						char* phpFileData = calloc( 1, lSize+1 );
						if( phpFileData )
						{
						    if( fread( phpFileData , lSize, 1 , fdTemp) != 1)
						    {
						      fclose(fdTemp) ;
						      free(phpFileData); 
						      //exit(1);
						    }
						}

						fclose(fdTemp); 
						

						/* Compile the PHP test program defined above */
						rc = ph7_compile_v2(
							pEngine,  /* PH7 engine */
							phpFileData, /* PHP test program */
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
						strcpy(&path[strlen(dirRoot)+strlen(reqline[1])], ".tmp");
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
						free(phpFileData); // Don't forget to free the allocated memory !
						
						fclose(file);

						if ( (fd=open(path, O_RDONLY))!=-1 ) { // file found
							send(socket, "HTTP/1.1 200 OK\n\n", 17, 0);

							while ( (bytes_read=read(fd, data_to_send, BYTES))>0 ) {
								write (socket, data_to_send, bytes_read);
							}
						} else { // file not found
							write(socket, "HTTP/1.1 404 Not Found\n", 23);

						}
						
					}

					else {
						send(socket, "HTTP/1.1 200 OK\n\n", 17, 0);

						while ( (bytes_read=read(fd, data_to_send, BYTES))>0 ) {
							write (socket, data_to_send, bytes_read);

						}
					}
				}
				else    write(socket, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
			}
		}
	}

	//Closing SOCKET
	shutdown (socket, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
	close(socket);
	close(fd);
	fprintf(logStream,"%s > ** End communication with %i **\n", getTime(), socket);
	fflush(logStream);
	socket=-1;
	return;
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
	    else if (strcmp(parameter, "WORKERS")==0){
	    	strncpy (workersTmp, value, MAXLEN);
	    	ret += 1;
	    }
	    //If config file contains more information 
	    else{
	      syslog(LOG_INFO, "%s/%s: Unknown name/value pair!\n", parameter, value);
	      ret += 1;
	      
	    }
	}
	//Validate if the parameters were found
	if(strcmp(logFileTmp,"")==0 && strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
		return EXIT_FAILURE;
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    workersNumber = atoi(workersTmp);
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	}
	else{
		if(strcmp(logFileTmp,"")>0){
			if (strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(logFileName, logFileTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(workersTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(port, portTmp);
	    		workersNumber = atoi(workersTmp);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
	    		workersNumber = atoi(workersTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(logFileName, logFileTmp);
	    		strcpy(port, portTmp);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(portTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
			}
			else{
				strcpy(logFileName, logFileTmp);
	    		workersNumber = atoi(workersTmp);
			}
		}
		else if(strcmp(portTmp,"")>0){
			if (strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(port, portTmp);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")>0){
				strcpy(port,portTmp);
				strcpy(dirRoot, rootTmp);
	    		workersNumber = atoi(workersTmp);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")==0){
				strcpy(port, portTmp);
				strcpy(dirRoot, rootTmp);
			}
			else{
				strcpy(port,portTmp);
	    		workersNumber = atoi(workersTmp);	
			}
		}
		else if (strcmp(rootTmp,"")>0){
			if(strcmp(workersTmp,"")==0){
				strcpy(dirRoot, rootTmp);
			}
			else{
				strcpy(dirRoot, rootTmp);
	    		workersNumber = atoi(workersTmp);
			}
		}
		else{
	    	workersNumber = atoi(workersTmp);
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
	    else if (strcmp(parameter, "WORKERS")==0){
	    	strncpy (workersTmp, value, MAXLEN);
	    	ret += 1;
	    }
	    //If config file contains more information 
	    else{
	      printf ("WARNING: %s/%s: Unknown name/value pair!\n", parameter, value);
	      ret += 1;
	    }
	}
	//Validate if the parameters were found
	if(strcmp(logFileTmp,"")==0 && strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0 && strcmp(workersTmp,"")==0){
		return EXIT_FAILURE;
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    workersNumber = atoi(workersTmp);
	    printf("Found LOGFILE:%s, PORT:%s , ROOT:%s and WORKERS:%d\n",logFileTmp,port,dirRoot, workersNumber);
	}
	else if(strcmp(logFileTmp,"")>0 && strcmp(portTmp,"")>0 && strcmp(rootTmp,"")>0){
		strcpy(logFileName, logFileTmp);
	    strcpy(port, portTmp);
	    strcpy(dirRoot, rootTmp);
	    printf("Found LOGFILE:%s, PORT:%s and ROOT:%s\n",logFileTmp,port,dirRoot);
	}
	else{
		if(strcmp(logFileTmp,"")>0){
			if (strcmp(portTmp,"")==0 && strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				printf("Just LOGFILE:%s found\n",logFileTmp);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(workersTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(port, portTmp);
	    		workersNumber = atoi(workersTmp);
				printf("Found LOGFILE:%s, PORT:%s and WORKERS:%d\n", logFileTmp, port, workersNumber);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")>0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
	    		workersNumber = atoi(workersTmp);
				printf("Found LOGFILE:%s, ROOT:%s and WORKERS:%d\n", logFileTmp, dirRoot, workersNumber);
			}
			else if (strcmp(portTmp,"")>0 && strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(logFileName, logFileTmp);
	    		strcpy(port, portTmp);
				printf("Found LOGFILE:%s and PORT:%s\n",logFileTmp,port);
			}
			else if (strcmp(rootTmp,"")>0 && strcmp(portTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(logFileName, logFileTmp);
				strcpy(dirRoot, rootTmp);
				printf("Found LOGFILE:%s and ROOT:%s\n",logFileTmp,dirRoot);
			}
			else{
				strcpy(logFileName, logFileTmp);
	    		workersNumber = atoi(workersTmp);
				printf("Found LOGFILE:%s and WORKERS:%d\n", logFileTmp, workersNumber);
			}
		}
		else if(strcmp(portTmp,"")>0){
			if (strcmp(rootTmp,"")==0 && strcmp(workersTmp,"")==0){
				strcpy(port, portTmp);
				printf("Just PORT:%s found\n",port);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")>0){
				strcpy(port,portTmp);
				strcpy(dirRoot, rootTmp);
	    		workersNumber = atoi(workersTmp);
				printf("Found PORT:%s, ROOT:%s and WORKERS:%d\n",port, dirRoot, workersNumber);
			}
			else if(strcmp(rootTmp,"")>0 && strcmp(workersTmp,"")==0){
				strcpy(port, portTmp);
				strcpy(dirRoot, rootTmp);
				printf("Found PORT:%s and ROOT:%s\n",port,dirRoot);
			}
			else{
				strcpy(port,portTmp);
	    		workersNumber = atoi(workersTmp);
				printf("Found PORT:%s and WORKERS:%d\n",port, workersNumber);	
			}
		}
		else if (strcmp(rootTmp,"")>0){
			if(strcmp(workersTmp,"")==0){
				strcpy(dirRoot, rootTmp);
				printf("Just ROOT:%s found\n",dirRoot);
			}
			else{
				strcpy(dirRoot, rootTmp);
	    		workersNumber = atoi(workersTmp);
				printf("Found ROOT:%s and WORKERS:%d\n", dirRoot, workersNumber);
			}
		}
		else{
	    	workersNumber = atoi(workersTmp);
			printf("Just WORKERS:%d\n", workersNumber);
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

		}
		// try to delete lockfile
		if (pidFileName != NULL) {
			unlink(pidFileName);

		}

		running = false;

		// server

		close(listenfd);

		// server

		kill(0, SIGKILL);

		// reset signal handling to default behavior
		signal(SIGINT, SIG_DFL);

	} else if (sig == SIGHUP) {
		fprintf(logStream, "%s > Debug: reloading daemon config file ...\n", getTime());
		// falta implementar
		fflush(logStream);
		readConfFile(1);

	} else if (sig == SIGCHLD) {
		wait(NULL); 
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

/*
 *	To handle the child process (workers)
 */
static int childProcessHandler(void* arg) {
	struct wcb* this = arg; // Gets its WCB instance
	this->pid = getpid(); // Sets its pid
	while( running ){
		while( !this->busy) { // Waiting a task to do while is not busy
			if(!running) return 0; // Exits if while waiting the execution ends
		}
		if(this->socket != -1) {
			respond(this->socket);
			//printf("Wssss\n");
		}
		this->busy = false; // resets its busy status
		//this->socket = -1; // resets the socket
	}

	return 0;
}

/*
 *	Makes a child process
 */
int forkWorker(void* ref){
	char* stack = malloc(STACK_SIZE); // creates a space for every child process
	// Clones is the same as fork but can share data between process
	return clone( &childProcessHandler, stack + STACK_SIZE, CLONE_FILES | CLONE_VM, ref );
}