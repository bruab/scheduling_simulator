#!/bin/bash

scripts/build_all_executables.sh

# run default first
echo -n "executable is bcast_default, time is "
scripts/bintree.run.100.sh bcast_default
echo "**********************************************************"

# now run ring and bintree
for chunk_size in 100000 200000 500000 1000000 2000000 10000000 100000000
do
	for version in bcast_ring_pipelined_isend bcast_bintree_pipelined_isend
	do
		echo -n "executable is $version, chunk size is $chunk_size, time is "
		scripts/bintree.run.100.sh $version -c $chunk_size
		echo "**********************************************************"
	done
done

