#!/usr/bin/env python
import sys
from src.compute_node import ComputeNode
from src.job import Job

def run_pbarc_jobs():
    with open("data/pbarc_jobs.tsv", 'r') as pbarcfile:
        for line in pbarcfile:
            if not ":" in line:
                # header or comment or blank or error or something
                continue
            print("ready to parse this line: " + line)
            fields = line.strip().split(":")
            # sample line:
            #all.q:compute-0-1.local:users:sgeib:assembly.qsub:2:sge:0:1395639825:1395639831:1395639831:0:0:0:0.107983:0.217966:2700.000000:0:0:0:0:26527:0:0:8.000000:24:0:0:0:226:157:NONE:defaultdepartment:orte:32:0:0.325949:0.000000:0.000000:-pe orte 32:0.000000:NONE:0.000000:0:0
            #  fields are 
            #  0: qname
            #  1: hostname
            #  2: group
            #  3: owner
            #  4: job_name
            #  5: job_number
            #  6: account
            #  7: priority
            #  8: submission_time
            #  9: start_time
            # 10: end_time
            # 11: failed - '0' means we good
            # 12: exit_status
            # 13: ru_wallclock - diff btwn end_time and start_time # see man getrusage for more
            # 14: ...ru stuff ..
            # ...
            # ??: project
            # ??: department
            # ??: granted_pe (parallel environment selected
            # ??: slots
            # ??: task_number
            # ??: cpu
            # ??: mem
            # ??: io
            # ??: category
            # ??: iow -- i/o wait time
            # ??: pe_taskid
            # ??: maxvmem
            # ??: arid - advance reservation id
            # ??: ar_submission_time
            

def main():
    run_pbarc_jobs()


###########################################

if __name__ == '__main__':
    main()
