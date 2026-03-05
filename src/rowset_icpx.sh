#!/bin/bash
if [ $# -lt 1 ] || [ $# -gt 2 ]
then
	echo "$0 N [ CFLAGS ]"
	exit
fi
icpx -O3 -xHost -DN=$1u $2 rowset.cpp -o rowset_$1.exe
