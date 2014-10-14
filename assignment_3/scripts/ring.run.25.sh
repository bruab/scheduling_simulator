#!/bin/bash

smpirun --cfg=smpi/bcast:mpich --cfg=smpi/running_power:1 -np 25 -platform xml_and_host_files/25_hosts/ring_25.xml -hostfile xml_and_host_files/25_hosts/hostfile_25 ./$@
