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
#include <meArmSerial.h>

// Macros
#define MAXLEN 80

// Global variables
char* confFileName;
char* appName;
char* cmmd;
int argX;
int argY;

int robot;

// Functions
int readConfFile(void);
int paser (char *);
char * trimValue(char *);
int isNumber (const char *);

// robotPlayer functions
void move(int,int);
void pick(void);
void drop(void);
void moveandpick(int,int);
void moveanddrop(int,int);
// Calls library
void initDevice(const char*,int);
void closeDevice();