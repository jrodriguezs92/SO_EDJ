#************************************************************
#
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 03/04/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

APP=mainTest
APP1=mainMutex
CC=gcc
LIBS=../lib
INCLUDE=../include
# Flags for linker
# Here you need to define wich thread library you want to use (-lpthread or -mypthread)
# * See also the header (<pthread.h> or <mypthread.h>)
LINKERFLAGS=-lmypthread 
OUT=../bin
OPTNS=-w

all: $(APP) $(APP1) setup

# Creates the destination folder
setup:
		mkdir -p $(OUT)

# Object File
$(APP).o:$(APP).c
		$(CC) $(OPTNS) -c $(APP).c -o $(APP).o -I$(INCLUDE)

# Linking with the libraries and binary generation
$(APP):$(APP).o setup
		$(CC) -o $(OUT)/$(APP) $(APP).o -L$(LIBS) $(LINKERFLAGS)

# Object File
$(APP1).o:$(APP1).c
		$(CC) $(OPTNS) -c $(APP1).c -o $(APP1).o -I$(INCLUDE)

# Linking with the libraries and binary generation
$(APP1):$(APP1).o setup
		$(CC) -o $(OUT)/$(APP1) $(APP1).o -L$(LIBS) $(LINKERFLAGS)

clean:
		rm -rf  $(OUT)
		rm -f *.o 
		rm -f *.so
		rm -f *.a

.PHONY:clean setup
