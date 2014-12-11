#!/usr/bin/env python
import sys
from src.compute_node import ComputeNode
from src.job import Job
from src.scheduler import Scheduler
from src.util import date_string_from_epoch_timestamp,\
        date_string_from_duration_in_seconds, jobs_from_accounting_file

SLOW_NODE_1_RUNNING_WATTS = 375.8
SLOW_NODE_1_IDLE_WATTS = 360.5
SLOW_NODE_1_CPUS = 48
SLOW_NODE_2_RUNNING_WATTS = 303
SLOW_NODE_2_IDLE_WATTS = 254
SLOW_NODE_2_CPUS = 48
FAST_NODE_RUNNING_WATTS = 254.095
FAST_NODE_IDLE_WATTS = 147
FAST_NODE_CPUS = 36


def run_historical_jobs(scheduling_algorithm, accounting_file):
    ## Create Nodes
    sys.stderr.write("Creating nodes ...\n")
    slow1 = ComputeNode(name='slow1', running_watts=SLOW_NODE_1_RUNNING_WATTS,
                        idle_watts=SLOW_NODE_1_IDLE_WATTS,
                        cpus=SLOW_NODE_1_CPUS)
    slow2 = ComputeNode(name='slow2', running_watts=SLOW_NODE_2_RUNNING_WATTS,
                        idle_watts=SLOW_NODE_2_IDLE_WATTS,
                        cpus=SLOW_NODE_2_CPUS)
    fast = ComputeNode(name='fast', running_watts=FAST_NODE_RUNNING_WATTS,
                        idle_watts=FAST_NODE_IDLE_WATTS,
                        cpus=FAST_NODE_CPUS)
    nodes = [slow1, slow2, fast]

    ## Read submission data into a list
    sys.stderr.write("Reading job submission data into memory ...\n")
    jobs = jobs_from_accounting_file(accounting_file)
    if not jobs:
        sys.stderr.write("Failed to read accounting file " + accounting_file +\
                            "; exiting.\n")
        sys.exit()
    sys.stderr.write("Got " + str(len(jobs)) + " jobs.\n")

    ## Get start and end times for simulation
    sys.stderr.write("Finding start and end times for period of study ...\n")
    period_of_study_begin = jobs[0].arrival_time
    period_of_study_end = jobs[-1].historical_end_time
    if not (period_of_study_begin and period_of_study_end):
        sys.stderr.write("Failed to find period of study; exiting.\n")
        sys.exit()
    # TODO is the following correct?
    period_of_study_duration = period_of_study_end - period_of_study_begin + 1

    ## Create Scheduler
    sys.stderr.write("Creating scheduler ...\n")
    scheduler = Scheduler(scheduling_algorithm, nodes, jobs)

    ## Run simulation
    sys.stderr.write("Initializing scheduler ...\n")
    scheduler.initialize(period_of_study_begin)
    sys.stderr.write("Beginning simulation ...\n")
    count = 1
    next_job_arrival_time = scheduler.get_next_job_arrival_time()
    for second in range(period_of_study_begin, period_of_study_end+2):
        print("\n\n***now simming second " + str(second) + "\n")
        if count % 3600 == 0:
            hours_to_go = int((period_of_study_end - second) / 3600)
            sys.stderr.write(str(hours_to_go) + " hours remaining ...\n")
        # fast forward if no jobs are arriving for a while
        if second >= next_job_arrival_time:
            scheduler.update(second)
        if not next_job_arrival_time:
            # have to run to completion
            scheduler.update(second)
        count += 1
        next_job_arrival_time = scheduler.get_next_job_arrival_time()
        print("just got next job arrival time " + str(next_job_arrival_time))

    ## Report results
    # print simulation info
    sim_start = date_string_from_epoch_timestamp(period_of_study_begin)
    sim_end = date_string_from_epoch_timestamp(period_of_study_end)
    sim_length = date_string_from_duration_in_seconds(period_of_study_duration)
    print("\n## SIMULATION INFORMATION ##\n")
    print("simulation start: " + sim_start)
    print("simulation end: " + sim_end)
    print("simulation duration (dd:hh:mm:ss): " + sim_length + "\n")

    # print job info
    print("\n## JOB INFORMATION ##\n")
    print("arrival_time\tstart_time\tcompletion_time\twait_time (dd:hh:mm:ss)\t" +
            "run_time (dd:hh:mm:ss)\tnode")
    print("------------\t----------\t---------------\t------------------\t----")
    print(scheduler.generate_job_report())

    # print header for node data
    print("\n## NODE INFORMATION ##\n")
    print("name\ttotal_compute_time (dd:hh:mm:ss)\ttotal_idle_time (dd:hh:mm:ss)\t" +
            "total_energy_consumption (kWh)")
    print("----\t------------------------\t---------------------\t" +
            "------------------------------")
    print(scheduler.generate_node_report())
            

def main():

    if len(sys.argv) < 3 or sys.argv[1] not in ["historical", "allfast"]:
        sys.stderr.write("usage: run_historical_jobs.py <scheduling algorithm> <accounting file>\n")
        sys.stderr.write(" where <scheduling algorithm> is either 'historical' or 'allfast'\n")
        sys.exit()

    run_historical_jobs(sys.argv[1], sys.argv[2])


###########################################

if __name__ == '__main__':
    main()
