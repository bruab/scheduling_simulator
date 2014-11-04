#!/bin/bash

scripts/build_all_executables.debug.sh

if [ $? == 0 ]
then
	scripts/run_this_executable.sh matmul_outerproduct 4
fi
