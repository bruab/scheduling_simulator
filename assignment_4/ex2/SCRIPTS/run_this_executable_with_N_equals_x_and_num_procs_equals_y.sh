#!/bin/bash

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:0.007 -np $3  -platform xml_and_host_files/1600_hosts/cluster_1600.xml -hostfile xml_and_host_files/1600_hosts/hostfile_1600 ./$1 $2
