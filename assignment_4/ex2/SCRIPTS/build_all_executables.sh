#!/bin/bash

# Remove any executables that might be hanging around
for executable in sequential_lu lu_simple lu_roundrobin lu_simulated
do
	if [[ -f $executable ]]
	then
		rm $executable
	fi
done

gcc sequential_lu.c -lm -O4 -DDEBUG -o sequential_lu
smpicc lu_simple.c -lm -O4 -DDEBUG -o lu_simple
smpicc lu_roundrobin.c -lm -O4 -DDEBUG -o lu_roundrobin
smpicc lu_simulated.c -lm -O4 -DDEBUG -o lu_simulated
