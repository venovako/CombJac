#!/bin/bash
g++$3 -Ofast -march=native -DN=$1u $2 diagne.cpp -o diagne_$1.exe
