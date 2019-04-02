#************************************************************
#
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 02/04/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

APP=bclient
CC=gcc
LIBS=../lib
INCLUDE=../include
# flags for linker
LINKERFLAGS=-lclient -lpthread
OUT=../bin
OPTNS=-Wall

all: $(APP) setup

# Creates the destination folder
setup:
		mkdir $(OUT)

# Object File
$(APP).o:$(APP).c
		$(CC) $(OPTNS) -c $(APP).c -o $(APP).o -I$(INCLUDE)

# Linking with the libraries and binary generation
$(APP):$(APP).o setup
		$(CC) -o $(OUT)/$(APP) $(APP).o -L$(LIBS) $(LINKERFLAGS)

clean:
		rm -rf  $(OUT)
		rm -f *.o 
		rm -f *.so
		rm -f *.a

.PHONY:clean setup