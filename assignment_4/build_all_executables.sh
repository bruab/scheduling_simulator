#!/bin/bash

# Remove any executables that might be hanging around
ls | grep mmult_ >> /dev/null
if [[ $? -eq 0 ]]
then
	rm mmult_*
fi

smpicc mmult.c -O4 -DVERSION=0 -o mmult_version0
smpicc mmult.c -O4 -DVERSION=1 -o mmult_version1
