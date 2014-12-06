#!/usr/bin/env python
import sys
from src.compute_node import ComputeNode
from src.job import Job

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

    ## Read submission data into a list
    jobs = jobs_from_accounting_file(accounting_file)
    if not jobs:
        sys.stderr.write("Failed to read accounting file " + accounting_file +\
                            "; exiting.\n")
        sys.exit()

    ## Get start and end times for simulation
    period_of_study_begin = jobs[0].arrival_time
    period_of_study_end = jobs[-1].historical_end_time

    print(period_of_study_begin)
    print(period_of_study_end)
    exit()
    ## Create Scheduler


    ## Run simulation


    ## Report results
    # print header for job data
    print("\n## JOB INFORMATION ##\n")
            # output arrival time, start time, completion time run time and energy cost for job
    print("arrival_time\tstart_time\tcompletion_time\trun_time (seconds)\tenergy_cost (kWh)")
    print("------------\t----------\t---------------\t------------------\t-----------")

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

            # keep track of overal time range
            # we assume that input data is chronological
            if not period_of_study_begin:
                period_of_study_begin = arrival_time
            period_of_study_end = end_time


            # ugly bit of business logic here
            if target_node == "compute-0-1.local":
                node = slow1
            elif target_node == "compute-0-2.local":
                node = slow2
            elif target_node == "compute-1-0.local":
                node = fast
            else:
                sys.stderr.write("couldn't figure out which node this line ran on; skipped it: " + line)
                continue

            # calculate energy cost in kWh
            energy_cost = ( run_time * node.running_watts / 3600 ) / 1000

            # output arrival time, start time, completion time run time and energy cost for job
            print(str(arrival_time) + "\t" + str(start_time) + "\t" +
                    str(end_time) + "\t" + str(run_time) + "\t" + str(energy_cost))
            # update node with start and end times, power consumption
            node.compute_times.append( (start_time, end_time) )

        # Finished read job submission file, time to wrap up
        period_of_study_duration = period_of_study_end - period_of_study_begin
        print("\n\n## NODE INFORMATION ##\n")
        print("pos start: " + str(period_of_study_begin))
        print("pos end: " + str(period_of_study_end))
        print("pos dur: " + str(period_of_study_duration) + "\n")
        for node in [slow1, slow2, fast]:
            total_compute_time = node.calculate_total_compute_time()
            running_kwh = ( total_compute_time * node.running_watts / 3600 ) / 1000
            total_idle_time = period_of_study_duration - total_compute_time
            idle_kwh = ( total_idle_time * node.idle_watts / 3600 ) / 1000
            total_kwh = idle_kwh + running_kwh
            print(node.name + "summary:")
            print("total compute time: " + str(total_compute_time))
            print("total idle time: " + str(total_idle_time))
            print("total energy consumption: " + str(total_kwh) + " kWh")
            print("")
            

def main():

    if len(sys.argv) < 2:
        sys.stderr.write("usage: run_historical_jobs.py <accounting file>\n")
        sys.exit()

    run_historical_jobs(sys.argv[1])


###########################################

if __name__ == '__main__':
    main()
