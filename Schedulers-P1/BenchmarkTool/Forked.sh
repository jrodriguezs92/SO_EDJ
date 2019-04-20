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
echo ">>Testing Forked"
#******************Forked Test*********************# 
#Few requests lightweight file jpg type
./bin/bclient localhost 8003 img.jpg 10 2

#600 requests of lightweight file jpg type
./bin/bclient localhost 8003 img.jpg 100 6 

#Few request 512MB file txt type
#./bin/bclient localhost 8003 file.txt 3 2

#20 requests 512MB file txt type
#./bin/bclient localhost 8003 file.txt 10 2

#GIF type file
./bin/bclient localhost 8003 img.gif 10 5

#Video mp4 type 
./bin/bclient localhost 8003 strokes.mp4 3 2

#HTL type file  
./bin/bclient localhost 8003 404.html  20 2