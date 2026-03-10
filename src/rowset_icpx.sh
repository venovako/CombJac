#!/bin/bash
if [ $# -lt 1 ] || [ $# -gt 2 ]
then
	echo "$0 ±N [ CFLAGS ]"
	exit
fi
if [ $1 -lt 0 ]
then
	let N=-$1
	F="rowset_all.cpp"
else
	N=$1
	F="rowset.cpp"
fi
icpx -O3 -xHost -DN=${N}u $2 $F -o rowset_$1.exe
