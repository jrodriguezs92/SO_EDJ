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

#define CONNMAX 1000
#define BYTES 1024
#define MAXLEN 80

char *dirRoot;
FILE* file;
FILE* logStream;
int listenfd;
int clients[CONNMAX];
char* confFileName;
char logFileTmp[MAXLEN];
char portTmp[MAXLEN];
char rootTmp[MAXLEN];
char* logFileName;
char* port;
char* pidFileName;
int pidFd;
char* appName;
int customLog;
int running;
int startDaemonized;
struct sockaddr_in clienteAddr;
socklen_t addrlen;

void startServer(char *);
void respond(int);
char * trimValue (char *);
int readConfFile(int);
int testConfFile(char *);
char* getTime(void);
void fatalError(const char *);
void handleSignal(int);
void daemonize();
void printHelp(void);
static int outputPHP(const void *, unsigned int, void *);
int isPHPRequest(char *);