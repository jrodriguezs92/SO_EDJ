#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 04/04/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

LIB_SERVER=server
LIB_PHP=ph7
LIB_NAME=mypthread
AUX_OBJ=structs
CC=gcc
INCLUDE=../include
AR=ar
OPTNS=-Wall

all:lib$(LIB_SERVER).a lib$(LIB_PHP).a lib$(LIB_NAME).a $(AUX_OBJ).o

$(LIB_SERVER).o:$(LIB_SERVER).c
		$(CC) $(OPTNS) -c $(LIB_SERVER).c -o $(LIB_SERVER).o -I$(INCLUDE)

# Make the library for the server
lib$(LIB_SERVER).a:$(LIB_SERVER).o
		$(AR) rcs lib$(LIB_SERVER).a $(LIB_SERVER).o

$(LIB_PHP).o:$(LIB_PHP).c
		$(CC) -c $(LIB_PHP).c -o $(LIB_PHP).o -I$(INCLUDE)

# Make the library for the php interpreter
lib$(LIB_PHP).a:$(LIB_PHP).o
		$(AR) rcs lib$(LIB_PHP).a $(LIB_PHP).o

$(LIB_NAME).o:$(LIB_NAME).c
		$(CC) $(OPTNS) -c $(LIB_NAME).c -o $(LIB_NAME).o -I$(INCLUDE)

# Aux for structs
$(AUX_OBJ).o:$(AUX_OBJ).c
		$(CC) $(OPTNS) -c $(AUX_OBJ).c -o $(AUX_OBJ).o -I$(INCLUDE)

# Make the library of mypthread
lib$(LIB_NAME).a:$(LIB_NAME).o $(AUX_OBJ).o
		$(AR) rcs lib$(LIB_NAME).a $(LIB_NAME).o $(AUX_OBJ).o

clean:
		rm -f *.o 
		rm -f *.a
		
.PHONY:clean