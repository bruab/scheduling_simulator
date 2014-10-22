#!/bin/bash

# Remove any executables that might be hanging around
ls | grep mmult_ >> /dev/null
if [[ $? -eq 0 ]]
then
	rm mmult_*
fi

ls | grep mat_mul__ >> /dev/null
if [[ $? -eq 0 ]]
then
	rm mat_mul__*
fi



smpicc mmult.c -O4 -DVERSION=0 -o mmult_version0
smpicc mmult.c -O4 -DVERSION=1 -o mmult_version1
smpicc mat_mul_init.c -O4 -DVERSION=0 -o mat_mul_init
