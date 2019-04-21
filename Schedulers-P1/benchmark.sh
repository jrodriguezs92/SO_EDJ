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

# Script absolute path
SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`

mkdir -p $SCRIPTPATH/BenchmarkTool/results

echo "> Compiling Benchmark"

make all --directory=$SCRIPTPATH/BenchmarkTool

echo "> Running Benchmark"

/$SCRIPTPATH/BenchmarkTool/FIFO.sh

/$SCRIPTPATH/BenchmarkTool/Forked.sh

/$SCRIPTPATH/BenchmarkTool/Threaded.sh

/$SCRIPTPATH/BenchmarkTool/Preforked.sh

/$SCRIPTPATH/BenchmarkTool/Prethreaded.sh
