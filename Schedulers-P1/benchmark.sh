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

mkdir -p results

make all

./FIFO.sh

./Forked.sh

./Threaded.sh

./Preforked.sh

./Prethreaded.sh
