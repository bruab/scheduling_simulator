#!/bin/bash

# Remove any executables that might be hanging around
# TODO check if they exist
rm mat_mul_init matmul_outerproduct

smpicc mmult.c -lm -O4 -DVERSION=0 -o mmult_version0
smpicc mmult.c -lm -O4 -DVERSION=1 -o mmult_version1
smpicc mat_mul_init.c -lm -O4 -DVERSION=0 -o mat_mul_init
smpicc matmul_outerproduct.c -lm -O4 -DVERSION=0 -o matmul_outerproduct
