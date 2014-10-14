#!/bin/bash

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1 -np 50 -platform xml_and_host_files/50_hosts/ring_50.xml -hostfile xml_and_host_files/50_hosts/hostfile_50 ./$@
