#!/bin/bash
if [ $# -lt 1 ] || [ $# -gt 3 ]
then
	echo "$0 N [ CFLAGS [ G++SUFFIX ] ]"
	exit
fi
g++$3 -Ofast -march=native -DN=$1u $2 diagne.cpp -o diagne_$1.exe
