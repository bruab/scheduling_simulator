#!/bin/bash

# Remove any executables that might be hanging around
# TODO check if they exist
rm mat_mul_init matmul_outerproduct

smpicc mat_mul_init.c -lm -O4 -DDEBUG -DVERSION=0 -o mat_mul_init
smpicc matmul_outerproduct.c -lm -O4 -DDEBUG -DVERSION=0 -o matmul_outerproduct
