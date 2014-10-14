#!/bin/bash

# Remove any executables that might be hanging around
ls | grep bcast_ >> /dev/null
if [[ $? -eq 0 ]]
then
	rm bcast_*
fi

smpicc bcast.c -DVERSION=0 -o bcast_default
smpicc bcast.c -DVERSION=1 -o bcast_naive
smpicc bcast.c -DVERSION=2 -o bcast_ring
smpicc bcast.c -DVERSION=3 -o bcast_ring_pipelined
smpicc bcast.c -DVERSION=4 -o bcast_ring_pipelined_isend
smpicc bcast.c -DVERSION=5 -o bcast_bintree_pipelined_isend
