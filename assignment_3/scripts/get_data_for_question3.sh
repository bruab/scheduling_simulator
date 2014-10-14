#!/bin/bash

scripts/build_all_executables.sh

for chunk_size in 100000 200000 500000 1000000 2000000 10000000 100000000
do
	for num_hosts in 25 50 100
	do
		echo -n "$num_hosts hosts, chunk size is $chunk_size, time is "
		scripts/ring.run.$num_hosts.sh bcast_ring_pipelined_isend -c $chunk_size
		echo "**********************************************************"
	done
done

