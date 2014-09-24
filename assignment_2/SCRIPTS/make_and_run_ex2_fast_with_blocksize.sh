#!/bin/bash

if [ $# -ne 2 ]
then
	echo "usage: make_and_etc.sh <blocksize> <nthreads>\n"
	exit -1
fi

rm exercise2_fast
cc -DBLOCK_SIZE=$1 -Wall -Ofast -fopenmp exercise2_fast.c -o exercise2_fast

# run program with $2 threads and 13 iterations
# (13 is the magic number that made the original take ~10sec)
./exercise2_fast $2 13
