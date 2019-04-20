#! /bin/bash

#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Daniela Hernández Alvarado (DaniHdez)
#
#	Last update: 26/02/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

#Formato de consulta
#./bclient <host> <port> <file> <nThreads> <nCycles>

# Script absolute path
SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`

echo ">>Testing FIFO"

#******************FIFO Test*********************# 
#Few requests lightweight file jpg type
/$SCRIPTPATH/bin/bclient localhost 8001 img.jpg 10 2

#100 requests of lightweight file jpg type
/$SCRIPTPATH/bin/bclient localhost 8001 img.jpg 1 100 

#Few request 512MB file txt type
#/$SCRIPTPATH/bin/bclient localhost 8001 file.txt 3 2

#20 requests 512MB file txt type
#/$SCRIPTPATH/bin/bclient localhost 8001 file.txt 10 2

#GIF type file
/$SCRIPTPATH/bin/bclient localhost 8001 img.gif 10 5

#Video mp4 type 
/$SCRIPTPATH/bin/bclient localhost 8001 strokes.mp4 3 2

#HTL type file  
/$SCRIPTPATH/bin/bclient localhost 8001 404.html  20 2