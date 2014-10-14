#!/bin/bash

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1 -np 64 -platform xml_and_host_files/64_hosts/cluster_64.xml -hostfile xml_and_host_files/64_hosts/hostfile_64 ./$@
