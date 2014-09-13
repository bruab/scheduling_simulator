#!/bin/bash

# compiles and runs exercise 1 with all six orderings, $1 represents the -O option to use
for x in {1..6}
do 
	cc -Wall -g -O$1 -DORDER=$x exercise1.c -o exercise1
	./exercise1
	rm exercise1
done
