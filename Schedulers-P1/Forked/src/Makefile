APP=WebServer
CC=gcc
LIBS=../lib
INCLUDE=../include
# flags for the server and the php interpreter
LINKERFLAGS=-lserver -lph7
OUT=../bin
OPTNS=-Wall

all: $(APP)

$(APP).o:$(APP).c
		$(CC) $(OPTNS) -c $(APP).c -o $(APP).o -I$(INCLUDE)

$(APP):$(APP).o
		$(CC) -o $(OUT)/$(APP) $(APP).o -L$(LIBS) $(LINKERFLAGS)

clean:
		rm -f  $(OUT)/$(APP)*
		rm -f *.o 
		rm -f *.so
		rm -f *.a

.PHONY:clean