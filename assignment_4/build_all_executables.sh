#!/bin/bash

# Remove any executables that might be hanging around
for executable in mat_mul_init matmul_outerproduct sequential_lu lu_simple
do
	if [[ -f $executable ]]
	then
		rm $executable
	fi
done

smpicc mat_mul_init.c -lm -O4 -o mat_mul_init
smpicc matmul_outerproduct.c -lm -O4 -o matmul_outerproduct
smpicc sequential_lu.c -lm -O4 -o sequential_lu
smpicc lu_simple.c -lm -O4 -o lu_simple
