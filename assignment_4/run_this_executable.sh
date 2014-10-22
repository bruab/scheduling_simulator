#!/bin/bash

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1 -np 64 -platform cluster_1600.xml -hostfile hostfile_1600 ./$@

