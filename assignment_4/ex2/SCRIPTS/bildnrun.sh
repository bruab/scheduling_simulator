#!/bin/bash

SCRIPTS/build_all_executables.sh

if [ $? == 0 ]
then
	SCRIPTS/run_this_executable.sh lu_simple 6
fi
