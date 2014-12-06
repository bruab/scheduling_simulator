#!/usr/bin/env python
import sys
from src.compute_node import ComputeNode
from src.job import Job
from src.scheduler import Scheduler

SLOW_NODE_1_RUNNING_WATTS = 375.8
SLOW_NODE_1_IDLE_WATTS = 360.5
SLOW_NODE_2_RUNNING_WATTS = 303
SLOW_NODE_2_IDLE_WATTS = 254
FAST_NODE_RUNNING_WATTS = 254.095
FAST_NODE_IDLE_WATTS = 147

def jobs_from_accounting_file(accounting_file):
    jobs = []
    with open(accounting_file, 'r') as accfile:
        for line in accfile:
            # sample line:
            # all.q:compute-0-1.local:users:sgeib:assembly.qsub:2:sge:0:1395639825:1395639831:1395639831:0:0:0:0.107983:0.217966:2700.000000:0:0:0:0:26527:0:0:8.000000:24:0:0:0:226:157:NONE:defaultdepartment:orte:32:0:0.325949:0.000000:0.000000:-pe orte 32:0.000000:NONE:0.000000:0:0
            if line.startswith("#"):
                # header or comment
                continue

            if not ":" in line:
                # header or blank or error or something
                continue

            fields = line.strip().split(":")
            if len(fields) < 10:
                # something is wrong
                sys.stderr.write("couldn't figure out how to parse this line, skipped it: " + line)
                continue

            # get run time, target node
            target_node = fields[1]
            arrival_time = int(fields[8])
            start_time = int(fields[9])
            end_time = int(fields[10])
            run_time = int(end_time) - int(start_time)

            # create Job and add it to list
            job = Job(arrival_time, start_time, end_time, target_node)
            jobs.append(job)
    return jobs


def run_historical_jobs(accounting_file):
    ## Create Nodes
    slow1 = ComputeNode(name='slow1', running_watts=SLOW_NODE_1_RUNNING_WATTS,
                        idle_watts=SLOW_NODE_1_IDLE_WATTS)
    slow2 = ComputeNode(name='slow2', running_watts=SLOW_NODE_2_RUNNING_WATTS,
                        idle_watts=SLOW_NODE_2_IDLE_WATTS)
    fast = ComputeNode(name='fast', running_watts=FAST_NODE_RUNNING_WATTS,
                        idle_watts=FAST_NODE_IDLE_WATTS)
    nodes = [slow1, slow2, fast]

    ## Read submission data into a list
    jobs = jobs_from_accounting_file(accounting_file)
    if not jobs:
        sys.stderr.write("Failed to read accounting file " + accounting_file +\
                            "; exiting.\n")
        sys.exit()

    ## Get start and end times for simulation
    period_of_study_begin = jobs[0].arrival_time
    period_of_study_end = jobs[-1].historical_end_time
    if not (period_of_study_begin and period_of_study_end):
        sys.stderr.write("Failed to find period of study; exiting.\n")
        sys.exit()
    period_of_study_duration = period_of_study_end - period_of_study_begin

    ## Create Scheduler
    scheduler = Scheduler(nodes, jobs)

    ## Run simulation
    scheduler.initialize(period_of_study_begin)
    for second in range(period_of_study_begin, period_of_study_end+1):
        scheduler.update(second)

    ## Report results
    # print simulation info
    print("\n## SIMULATION INFORMATION ##\n")
    print("simulation start: " + str(period_of_study_begin))
    print("simulation end: " + str(period_of_study_end))
    print("simulation dur: " + str(period_of_study_duration) + "\n")

    # print job info
    print("\n## JOB INFORMATION ##\n")
    print("arrival_time\tstart_time\tcompletion_time\trun_time (seconds)\t")
    print("------------\t----------\t---------------\t------------------\t-----------")
    print(scheduler.generate_job_report())

    # print header for node data
    print("\n\n## NODE INFORMATION ##\n")
    print("name\ttotal_compute_time (sec)\ttotal_idle_time (sec)\t" +
            "total_energy_consumption (kWh)")
    print("----\t------------------------\t---------------------\t" +
            "------------------------------")
    print(scheduler.generate_node_report())
            

def main():

    if len(sys.argv) < 2:
        sys.stderr.write("usage: run_historical_jobs.py <accounting file>\n")
        sys.exit()

    run_historical_jobs(sys.argv[1])


###########################################

if __name__ == '__main__':
    main()
