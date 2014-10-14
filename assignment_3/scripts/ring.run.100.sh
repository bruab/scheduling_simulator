#!/bin/bash

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1 -np 100 -platform xml_and_host_files/100_hosts/ring_100.xml -hostfile xml_and_host_files/100_hosts/hostfile_100 ./$@
