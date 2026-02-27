#!/bin/bash
clang++ -O3 -march=native -integrated-as -DN=$1u $2 rowset.cpp -o rowset_$1.exe
