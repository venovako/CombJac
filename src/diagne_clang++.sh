#!/bin/bash
if [ $# -lt 1 ] || [ $# -gt 2 ]
then
	echo "$0 N [ CFLAGS ]"
	exit
fi
clang++ -O3 -march=native -integrated-as -DN=$1u $2 diagne.cpp -o diagne_$1.exe
