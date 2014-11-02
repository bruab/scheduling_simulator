#!/bin/bash

./build_all_executables.sh

if [ $? == 0 ]
then
	./run_this_executable.sh matmul_outerproduct 4
fi
