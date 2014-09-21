#!/bin/bash

# compiles and runs exercise 3 with all six orderings, $1 represents the -O option to use
for x in {1..6}
do 
	cc -Wall -g -O$1 -DORDER=$x exercise3.c -o exercise3
	./exercise3
	rm exercise3
done
