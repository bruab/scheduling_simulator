#!/bin/bash

# compiles and runs all three versions of exercise 1
for x in {1..3}
do 
	cc -Wall -g -Ofast -fopenmp -DPARALLEL_LOOP=$x exercise1.c -o exercise1
	./exercise1
	rm exercise1
done
