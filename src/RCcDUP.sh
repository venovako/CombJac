#!/bin/bash
# Usage: ./RCcDUP.sh N CXX
# M = 2 * N
N=$1
if [ $N -lt 2 ]
then
	echo "$N < 2"
	exit
fi
let M=N%2
if [ $M -eq 1 ]
then
	echo "$N odd"
	exit
fi
if [ $N -lt 10 ]
then
	KN="unsigned char"
	AN="RCc00$N"
elif [ $N -lt 100 ]
then
	KN="unsigned char"
	AN="RCc0$N"
elif [ $N -le 256 ]
then
	KN="unsigned char"
	AN="RCc$N"
elif [ $N -le 65536 ]
then
	KN="unsigned short"
	AN="RCc$N"
else
	KN="unsigned"
	AN="RCc$N"
fi
let SN=N-1
let PN=N/2
let M=N*2
if [ $M -lt 10 ]
then
	KM="unsigned char"
	FK="int8"
	AM="RCc00$M"
elif [ $M -lt 100 ]
then
	KM="unsigned char"
	FK="int8"
	AM="RCc0$M"
elif [ $M -lt 128 ]
then
	KM="unsigned char"
	FK="int8"
	AM="RCc$M"
elif [ $M -le 256 ]
then
	KM="unsigned char"
	FK="int16"
	AM="RCc$M"
elif [ $M -lt 32768 ]
then
	KM="unsigned short"
	FK="int16"
	AM="RCc$M"
elif [ $M -le 65536 ]
then
	KM="unsigned short"
	FK="int32"
	AM="RCc$M"
else
	KM="unsigned"
	FK="int32"
	AM="RCc$M"
fi
let SM=M-1
let PM=M/2
$2 -O3 -fopenmp -DN=${N}u -DHN="\"$AN.hpp\"" -DKN="$KN" -DAN=$AN -DSN=${SN}u -DPN=${PN}u -DM=${M}u -DHM="\"$AM.hpp\"" -DKM="$KM" -DAM=$AM -DSM=${SM}u -DPM=${PM}u -DKK="\"$KM\"" -DAK="\"$AM\"" -DFM="\"$AM.f90\"" -DFK="\"$FK\"" RCcDUP.cpp -o RCcDUP.exe
echo -n "$KN $AN[$SN][$PN][2] ==> $KM $AM[$SM][$PM][2] ... "
unset N KN AN SN PN M KM AM SM PM
./RCcDUP.exe
echo "done"
