#!/bin/bash
icpx -O3 -xHost -DN=$1u $2 diagne.cpp -o diagne_$1.exe
