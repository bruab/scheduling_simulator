#!/bin/bash

# Remove any executables that might be hanging around
if [[ -f bcast_default ]]
then
	rm bcast_default
fi

if [[ -f bcast_naive ]]
then
	rm bcast_naive
fi

if [[ -f bcast_ring ]]
then
	rm bcast_ring
fi

if [[ -f bcast_ring_pipelined ]]
then
	rm bcast_ring_pipelined
fi

smpicc bcast.c -DVERSION=0 -o bcast_default
smpicc bcast.c -DVERSION=1 -o bcast_naive
smpicc bcast.c -DVERSION=2 -o bcast_ring
smpicc bcast.c -DVERSION=3 -o bcast_ring_pipelined
