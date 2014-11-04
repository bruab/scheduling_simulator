#!/bin/bash

# Remove any executables that might be hanging around
for executable in mat_mul_init matmul_outerproduct 
do
	if [[ -f $executable ]]
	then
		rm $executable
	fi
done

smpicc mat_mul_init.c -lm -O4 -DDEBUG -o mat_mul_init
smpicc matmul_outerproduct.c -lm -O4 -DDEBUG -o matmul_outerproduct
