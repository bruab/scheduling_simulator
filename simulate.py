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
DOLLARS_PER_KILOWATT_HOUR = 0.326671


def simulate(scheduling_algorithm, accounting_file, verbose=False):
    ## Create Nodes
    if verbose:
        sys.stderr.write("Creating nodes ...\n")
    slow1 = ComputeNode(name='slow1', running_watts=SLOW_NODE_1_RUNNING_WATTS,
                        idle_watts=SLOW_NODE_1_IDLE_WATTS,
                        cpus=SLOW_NODE_1_CPUS, cost_per_kwh=DOLLARS_PER_KILOWATT_HOUR,
                        verbose=verbose)
    slow2 = ComputeNode(name='slow2', running_watts=SLOW_NODE_2_RUNNING_WATTS,
                        idle_watts=SLOW_NODE_2_IDLE_WATTS,
                        cpus=SLOW_NODE_2_CPUS, cost_per_kwh=DOLLARS_PER_KILOWATT_HOUR,
                        verbose=verbose)
    fast = ComputeNode(name='fast', running_watts=FAST_NODE_RUNNING_WATTS,
                        idle_watts=FAST_NODE_IDLE_WATTS,
                        cpus=FAST_NODE_CPUS, cost_per_kwh=DOLLARS_PER_KILOWATT_HOUR,
                        verbose=verbose)
    nodes = [slow1, slow2, fast]

    ## Read submission data into a list
    if verbose:
        sys.stderr.write("Reading job submission data into memory ...\n")
    jobs = jobs_from_accounting_file(accounting_file)
    if not jobs:
        sys.stderr.write("Failed to read accounting file " + accounting_file +\
                            "; exiting.\n")
        sys.exit()
    sys.stderr.write("Got " + str(len(jobs)) + " jobs.\n")

    period_of_study_begin = jobs[0].arrival_time

    ## Create Scheduler
    if verbose:
        sys.stderr.write("Creating scheduler ...\n")
    scheduler = Scheduler(scheduling_algorithm, nodes, jobs, verbose)

    ## Run simulation
    if verbose:
        sys.stderr.write("Initializing scheduler ...\n")
    scheduler.initialize(period_of_study_begin)
    if verbose:
        sys.stderr.write("Beginning simulation ...\n")
    current_second = period_of_study_begin

    while scheduler.jobs_remaining():
        if verbose and current_second % 3600 == 0:
            hours_elapsed = int((current_second - period_of_study_begin) / 3600)
            sys.stderr.write(str(hours_elapsed) + " sim hours elapsed ...\n")
            sys.stderr.write("scheduler has " + str(scheduler.jobs_remaining()) +
                            " jobs remaining\n")
        scheduler.update(current_second)
        current_second += 1

    period_of_study_end = current_second
    period_of_study_duration = period_of_study_end - period_of_study_begin

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
            "total_energy_consumption (kWh)\ttotal_energy_cost")
    print("----\t------------------------\t---------------------\t" +
            "------------------------------")
    print(scheduler.generate_node_report())
            

def main():

    if len(sys.argv) < 3 or sys.argv[1] not in ["historical", "allfast", 
                                                "greenfirst2nodes", "greenfirst3nodes"]:
        sys.stderr.write("usage: simulate.py <scheduling algorithm> <accounting file> [-v]\n")
        sys.stderr.write(" where <scheduling algorithm> is either 'historical' or 'allfast'\n")
        sys.stderr.write(" -v flag gives verbose output\n")
        sys.exit()

    verbose = False
    if "-v" in sys.argv:
        verbose = True

    simulate(sys.argv[1], sys.argv[2], verbose)


###########################################

if __name__ == '__main__':
    main()
