/**
 * Copyright (C) 2019 
 * Área Académica de Ingeniería en Computadoras, ITCR, Costa Rica
 *
 * Interpreter
 * Heeader File
 *
 * @Author: Jeremy Rodriguez
 * @Date  : 22.05.2019
 */

// Libraries
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Macros
#define MAXLEN 80

// Global variables
char* confFileName;
char* appName;
char* cmmd;
char* argX;
char* argY;

// Functions
void printHelp(void);
int readConfFile(void);
int paser (char *);
char * trimValue(char *);
int isNumber (const char *);