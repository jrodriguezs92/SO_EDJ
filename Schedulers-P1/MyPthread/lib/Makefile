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

LIB_NAME=mypthread
AUX_OBJ=structs
CC=gcc
INCLUDE=../include
AR=ar
OPTNS=-Wall

all:lib$(LIB_NAME).a $(AUX_OBJ).o

$(LIB_NAME).o:$(LIB_NAME).c
		$(CC) $(OPTNS) -c $(LIB_NAME).c -o $(LIB_NAME).o -I$(INCLUDE)

$(AUX_OBJ).o:$(AUX_OBJ).c
		$(CC) $(OPTNS) -c $(AUX_OBJ).c -o $(AUX_OBJ).o -I$(INCLUDE)

# Make the library
lib$(LIB_NAME).a:$(LIB_NAME).o $(AUX_OBJ).o
		$(AR) rcs lib$(LIB_NAME).a $(LIB_NAME).o $(AUX_OBJ).o

clean:
		rm -f *.o 
		rm -f *.a
		
.PHONY:clean
