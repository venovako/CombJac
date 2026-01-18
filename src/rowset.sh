#!/bin/bash
# Usage: ./rowset.sh (clang++|g++|icpx) [compiler_args [g++_suffix]]
# Example (macOS): ./rowset.sh g++ "-DNDEBUG -Wno-stringop-overflow" -15
for ((N=2;N<=42;N+=2))
do
	./rowset_$1.sh $N "$2" $3
	echo "RUN $N"
	./rowset_$N.exe > rowset_$N.out 2> rowset_$N.err
done
unset N
