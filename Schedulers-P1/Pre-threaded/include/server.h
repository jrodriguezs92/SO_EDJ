/*server.h*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Daniela Hernández A (DaniHdez)

		Programming Language: C
		Version: 1.0
		Last Update: 05/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <ph7.h>
#include <pthread.h>

#define MAXLEN 80
#define CONEXMAX 1000
#define BYTES 8
#define MSGLEN 9999
#define MAX_QUEUE 1000000

#ifndef HAVE_GETRUSAGE_PROTO
	int getrusage(int, struct rusage *);
#endif

// Global variables
int running;
int delay;
int pidFd;
int startDaemonized;
char* confFileName;
char* pidFileName;
char* appName;
FILE* logStream;
char* logFileName;
int sockfd; // fd=file descriptor
struct sockaddr_in clienteAddr;
socklen_t addrLen;
char* dirRoot;
int* clients;
char* port;
char* scheduler;
int schedulerID;
int customLog;
int workersNumber;
char logFileTmp[MAXLEN];
char portTmp[MAXLEN];
char rootTmp[MAXLEN];
char schedulerTmp[MAXLEN];
char workersTmp[MAXLEN];
FILE* file;
int numOfThreads;
//pthread_mutex_t mutexLock;

// Tread structure to handle the pre-threaded function
typedef struct{
	pthread_t tid;
	int t_count;
}THREAD;
THREAD *thr_ctl;

#define MAXNCLI 32
int clifd[MAXNCLI], iget, iput;
pthread_mutex_t clifd_mutex;
pthread_cond_t clifd_cond;


// Server prototypes
void startServer(char *);
void *requestResponse(void *);
int readConfFile(int);
int testConfFile(char *);
void handleSignal(int);
void daemonize();
void printHelp(void);
char* getTime(void);
void fatalError(const char*);
int isPHPRequest(char*);
struct args {int sslot; };
int isMultimedia(char*);
void sig_int(int);
void pr_cpu_time(void);
/*server.h*/
