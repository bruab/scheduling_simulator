#!/bin/bash

scripts/build_all_executables.sh

for network in fattree cluster
do

	# run default first
	echo -n "executable is bcast_default, network is $network, time is "
	scripts/$network.run.64.sh bcast_default
	echo "**********************************************************"

	# now run ring and bintree
	for chunk_size in 100000 200000 500000 1000000 2000000 10000000 100000000
	do
		for version in bcast_ring_pipelined_isend bcast_bintree_pipelined_isend
		do
			echo -n "executable is $version, network is $network, chunk size is $chunk_size, time is "
			scripts/$network.run.64.sh $version -c $chunk_size
			echo "**********************************************************"
		done
	done
done

