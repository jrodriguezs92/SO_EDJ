#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 26/02/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

APP=server
CC=gcc
INCLUDE=../include
AR=ar
OPTNS=-Wall

all:lib$(APP).a

$(APP).o:$(APP).c
		$(CC) $(OPTNS) -c $(APP).c -o $(APP).o -I$(INCLUDE)

lib$(APP).a:$(APP).o
		$(AR) rcs lib$(APP).a $(APP).o

clean:
		rm -f *.o 
		rm -f *.a
		
.PHONY:clean