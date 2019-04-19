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

echo "> Running Benchmark"

cd BenchmarkTool

make all
#Formato de consulta
#./bclient <host> <port> <file> <nThreads> <nCycles>

#******************FIFO Test*********************# 
#Few requests lightweight file png type
./bin/bclient localhost 8001 img.jpg 10 2

#1000 requests of lightweight file png type
./bin/bclient localhost 8001 img.jpg 100 10 

#GIF type file
./bin/bclient localhost 8001 img.gif 10 5

#Video mp4 type 
./bin/bclient localhost 8001 strokes.mp4 3 2

#HTL type file  
./bin/bclient localhost 8001 404.html  20 2

#******************Forked Test*********************# 
#Few requests lightweight file png type
./bin/bclient localhost 8003 img.jpg 10 2

#1000 requests of lightweight file png type
./bin/bclient localhost 8003 img.jpg 100 10 

#GIF type file
./bin/bclient localhost 8003 img.gif 10 5

#Video mp4 type 
./bin/bclient localhost 8003 strokes.mp4 3 2

#HTL type file  
./bin/bclient localhost 8003 404.html  20 2

#******************Threaded Test*********************# 
#Few requests lightweight file png type
./bin/bclient localhost 8005 img.jpg 10 2

#1000 requests of lightweight file png type
./bin/bclient localhost 8005 img.jpg 100 10 

#GIF type file
./bin/bclient localhost 8005 img.gif 10 5

#Video mp4 type 
./bin/bclient localhost 8005 strokes.mp4 3 2

#HTL type file  
./bin/bclient localhost 8005 404.html  20 2

#******************Prethreaded Test*********************# 
#Few requests lightweight file png type
./bin/bclient localhost 8007 img.jpg 10 2

#1000 requests of lightweight file png type
./bin/bclient localhost 8007 img.jpg 100 10 

#GIF type file
./bin/bclient localhost 8007 img.gif 10 5

#Video mp4 type 
./bin/bclient localhost 8007 strokes.mp4 3 2

#HTL type file  
./bin/bclient localhost 8007 404.html  20 2

#******************Preforked Test*********************# 
#Few requests lightweight file png type
#Few requests lightweight file png type
./bin/bclient localhost 8009 img.jpg 10 2

#1000 requests of lightweight file png type
./bin/bclient localhost 8009 img.jpg 100 10 

#GIF type file
./bin/bclient localhost 8009 img.gif 10 5

#Video mp4 type 
./bin/bclient localhost 8009 strokes.mp4 3 2

#HTL type file  
./bin/bclient localhost 8009 404.html  20 2