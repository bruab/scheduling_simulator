#!/bin/bash

# Remove any executables that might be hanging around
if [[ -f question1 ]]
then
	rm question1
fi

if [[ -f question1_naive ]]
then
	rm question1_naive
fi

smpicc question1.c -DVERSION=default -o question1
smpicc question1.c -DVERSION=naive -o question1_naive
