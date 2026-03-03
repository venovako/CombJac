#!/bin/bash
if [ $# -lt 2 ] || [ $# -gt 3 ]
then
	echo "$0 N CXX [ W ]"
	exit
fi
N=$1
if [ $N -lt 4 ]
then
	echo "$N < 4"
	exit
elif [ $N -lt 10 ]
then
	T="RCc00$N"
elif [ $N -lt 100 ]
then
	T="RCc0$N"
else
	T="RCc$N"
fi
if [ $# -eq 3 ]
then
	$2 -O3 -DNDEBUG -DN=${N}u -DH="\"$T.hpp\"" -DT=$T -DV="\"$3\"" RCcSRC.cpp -o RCcSRC.exe
else
	$2 -O3 -DNDEBUG -DN=${N}u -DH="\"$T.hpp\"" -DT=$T RCcSRC.cpp -o RCcSRC.exe
fi
unset N T
./RCcSRC.exe
