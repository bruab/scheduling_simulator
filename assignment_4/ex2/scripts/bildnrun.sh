#!/bin/bash

scripts/build_all_executables.sh

if [ $? == 0 ]
then
	scripts/run_this_executable.sh lu_simple 6
fi
