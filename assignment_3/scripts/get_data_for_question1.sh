#!/bin/bash

scripts/build_all_executables.sh

for x in default naive ring
do
	echo -n $x": "
	scripts/ring.run.100.sh bcast_$x 2> /dev/null
	echo "***"
done
