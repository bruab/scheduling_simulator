#!/bin/bash

# Remove any executables that might be hanging around
for executable in sequential_lu lu_simple
do
	if [[ -f $executable ]]
	then
		rm $executable
	fi
done

gcc sequential_lu.c -lm -O4 -DDEBUG -o sequential_lu
smpicc lu_simple.c -lm -O4 -DDEBUG -o lu_simple