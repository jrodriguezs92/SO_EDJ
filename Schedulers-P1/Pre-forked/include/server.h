/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernández A (daniha)

		Programming Language: C
		Version: 1.0
		Last Update: 18/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <ph7.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/utsname.h>

#define CONNMAX 1000
#define BYTES 1024
#define MAXLEN 80
#define STACK_SIZE (1024*1024)
#define MSGLEN 2048

// Worker Control Block	
typedef struct wcb{
	int pid; // To know wich worker is
	bool busy; // To know if the worker is busy
	int socket; // To sets which socket will work on
} wcb;
typedef struct wcb* WCB;

// Global variables
char *dirRoot;
FILE* file;
FILE* logStream;
int listenfd;
char* confFileName;
char logFileTmp[MAXLEN];
char portTmp[MAXLEN];
char rootTmp[MAXLEN];
char schedulerTmp[MAXLEN];
char workersTmp[MAXLEN];
char* logFileName;
char* port;
char* scheduler;
int workersNumber;
int schedulerID;
char* pidFileName;
int pidFd;
char* appName;
int customLog;
bool running;
int startDaemonized;
struct sockaddr_in clienteAddr;
socklen_t addrlen;
int workersAllowed;
WCB* workers;

// Instructions Prototypes
void startServer(char *);
char * trimValue (char *);
int readConfFile(int);
int testConfFile(char *);
char* getTime(void);
void fatalError(const char *);
void handleSignal(int);
void daemonize();
void printHelp(void);
int isPHPRequest(char *);
int forkWorker(void*);