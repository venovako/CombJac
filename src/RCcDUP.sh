#!/bin/bash
# Usage: ./RCcDUP.sh N K CXX
# M = N * (2 ^ K)
N=$1
if [ $N -lt 2 ]
then
    echo "$N < 2"
    exit
fi
if [ $N -gt 42 ]
then
    echo "$N > 42"
    exit
fi
let K=N%2
if [ $K -eq 1 ]
then
    echo "$N odd"
    exit
fi
let K=N/2
let M=K%2
if [ $M -eq 0 ]
then
    echo "$N/2=$K even"
    exit
fi
if [ $2 -lt 0 ]
then
    echo "$2 < 0"
    exit
fi
if [ $N -lt 10 ]
then
    AN="RCc00$N"
elif [ $N -lt 100 ]
then
    AN="RCc0$N"
else
    AN="RCc$N"
fi
let SN=N-1
let PN=N/2
M=$N
for ((K=$2; K>0; --K))
do
    let M*=2
done
if [ $M -lt 10 ]
then
    AM="RCc00$M"
    K="unsigned char"
elif [ $M -lt 100 ]
then
    AM="RCc0$M"
    K="unsigned char"
elif [ $M -lt 256 ]
then
    AM="RCc$M"
    K="unsigned char"
elif [ $M -lt 65536 ]
then
    AM="RCc$M"
    K="unsigned short"
else
    AM="RCc$M"
    K="unsigned"
fi
let SM=M-1
let PM=M/2
$3 -O3 -DN=${N}u -DHN="\"$AN.hpp\"" -DKN="unsigned char" -DAN=$AN -DSN=${SN}u -DPN=${PN}u -DM=${M}u -DHM="\"$AM.hpp\"" -DKM="$K" -DAM=$AM -DSM=${SM}u -DPM=${PM}u RCcDUP.cpp -o RCcDUP.exe
echo -n "unsigned char $AN[$SN][$PN][2] |-> $K $AM[$SM][$PM][2] ... "
unset N AN SN PN M AM SM PM K
./RCcDUP.exe
echo "done"
