#!/bin/bash

# Remove any executables that might be hanging around
for executable in sequential_lu lu_simple
do
	if [[ -f $executable ]]
	then
		rm $executable
	fi
done

smpicc sequential_lu.c -lm -O4 -o sequential_lu
smpicc lu_simple.c -lm -O4 -o lu_simple
