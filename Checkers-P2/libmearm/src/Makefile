#********************************************************
#			Instituto Tecnológico de Costa Rica
#				Computer Engineering
#
#		Programmer: Esteban Agüero Pérez (estape11)
#		Version: 1.0
#		Last Update: 30/05/2019
#
#				Operating Systems Principles
#				Professor. Diego Vargas
#********************************************************

BIN=roboticPlayer
APP=main
CC=gcc
LIBS=../lib
INCLUDE=../include
LINKERFLAGS=-lroboticPlayer
OUT=../bin
OPTNS=-Wall

all: $(BIN)

$(APP).o:$(APP).c
		$(CC) $(OPTNS) -c $(APP).c -o $(APP).o -I$(INCLUDE)

$(BIN):$(APP).o
		$(CC) -o $(OUT)/$(BIN) $(APP).o -L$(LIBS) $(LINKERFLAGS)

clean:
		rm -f  $(OUT)/$(BIN)*
		rm -f *.o 
		rm -f *.so
		rm -f *.a

.PHONY:clean