#!/usr/bin/env python
import sys
from src.compute_node import ComputeNode
from src.job import Job

SLOW_NODE_1_WATTS = 440
SLOW_NODE_2_WATTS = 410
FAST_NODE_WATTS = 320

def run_historical_jobs(accounting_file):
    # Create Nodes
    slow1 = ComputeNode(name='slow1', watts_per_second=SLOW_NODE_1_WATTS)
    slow2 = ComputeNode(name='slow2', watts_per_second=SLOW_NODE_2_WATTS)
    fast = ComputeNode(name='fast', watts_per_second=FAST_NODE_WATTS)

    # print header for job data
    print("\n## JOB INFORMATION ##\n")
            # output arrival time, start time, completion time run time and energy cost for job
    print("arrival_time\tstart_time\tcompletion_time\trun_time\tenergy_cost")
    print("------------\t----------\t---------------\t--------\t-----------")
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
            submission_time = fields[8]
            start_time = fields[9]
            end_time = fields[10]
            run_time = int(end_time) - int(start_time)

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

            # calculate energy cost
            energy_cost = run_time * node.watts_per_second

            # output arrival time, start time, completion time run time and energy cost for job
            print(str(submission_time) + "\t" + str(start_time) + "\t" +
                    str(end_time) + "\t" + str(run_time) + "\t" + str(energy_cost))
            # update node with runtime, power consumption
            node.wattages.append(energy_cost)
            node.compute_times.append(run_time)
            # TODO add node idle time

        print("\n\n## NODE INFORMATION ##\n")
        for node in [slow1, slow2, fast]:
            # TODO output summary stats for runtimes, idle times, power consumption
            print(node.name + "summary:")
            print("wattages: " + str(node.wattages))
            print("compute times: " + str(node.compute_times))
            

def main():

    if len(sys.argv) < 2:
        sys.stderr.write("usage: run_historical_jobs.py <accounting file>\n")
        sys.exit()

    run_historical_jobs(sys.argv[1])


###########################################

if __name__ == '__main__':
    main()
