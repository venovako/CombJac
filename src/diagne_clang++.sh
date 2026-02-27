#!/bin/bash
clang++ -O3 -march=native -integrated-as -DN=$1u $2 diagne.cpp -o diagne_$1.exe
