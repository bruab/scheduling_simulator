#!/usr/bin/env python

import sys
from src.util import date_string_from_epoch_timestamp, date_string_from_duration_in_seconds
from src.compute_node import calculate_compute_time

class Scheduler:

    def __init__(self, algorithm, nodes, jobs, verbose=False):
        self.algorithm = algorithm
        self.nodes = {}
        self.verbose = verbose
        for node in nodes:
            self.nodes[node.name] = node
        self.future_jobs = []
        for job in jobs:
            # skip jobs that ran on nodes we don't know about
            if not self.get_node_from_historical_node_name(job.historical_node):
                continue
            self.future_jobs.append(job)
        self.current_time = None
        self.unscheduled_jobs = []
        self.scheduled_jobs = []
        self.completed_jobs = []

    def initialize(self, init_time):
        self.current_time = init_time
        for node in self.nodes.values():
            node.initialize(init_time)

    def update(self, newtime):
        if self.verbose:
            sys.stderr.write("\tsched.update() here, updating at newtime " + str(newtime) + "\n")
        # get completed jobs from nodes
        for node in self.nodes.values():
            completed_jobs = node.update(newtime)
            if self.verbose:
                if completed_jobs:
                    sys.stderr.write("\tnode " + str(node) + "just returned these completed jobs: " 
                            + str(completed_jobs) + "\n")
                else:
                    sys.stderr.write("\tno completed jobs from node " + str(node) + "\n")
            if completed_jobs:
                self.completed_jobs += completed_jobs
                self.scheduled_jobs = [j for j in self.scheduled_jobs if j not in completed_jobs]
        # if any future jobs arrive this second add them to unscheduled_jobs list
        while self.future_jobs and self.future_jobs[0].arrival_time <= newtime:
            if self.verbose:
                sys.stderr.write("About to move job " + str(self.future_jobs[0]) +
                                 " from future_jobs to unscheduled_jobs\n")
            self.unscheduled_jobs.append(self.future_jobs.pop(0))

        # try to schedule jobs
        while self.unscheduled_jobs:
            if self.assign_job(self.unscheduled_jobs[0], newtime):
                if self.verbose:
                    sys.stderr.write("just assigned job " + 
                            str(self.unscheduled_jobs[0]) + "\n")
                self.scheduled_jobs.append(self.unscheduled_jobs.pop(0))
            else:
                break

        # update time
        self.current_time = newtime

    def jobs_remaining(self):
        count = 0
        if self.future_jobs:
            count += len(self.future_jobs)
        if self.scheduled_jobs:
            count += len(self.scheduled_jobs)
        return count

    def get_next_job_arrival_time(self):
        if not self.future_jobs:
            sys.stderr.write("******************** NO PENDING JOBS\n\n")
            return None
        else:
            return self.future_jobs[0].arrival_time

    def generate_job_report(self):
        # arrival_time\tstart_time\tcompletion_time\twait_time\trun_time\tnode
        report = ""
        for job in self.completed_jobs:
            stats = [job.arrival_time, job.start_time, job.end_time]
            stats = [date_string_from_epoch_timestamp(x) for x in stats]
            stats.append(date_string_from_duration_in_seconds(job.start_time - job.arrival_time+1))
            stats.append(date_string_from_duration_in_seconds(job.end_time - job.start_time+1))
            stats.append(job.node_name)
            report += "\t".join(stats) + "\n"

        if self.future_jobs or self.scheduled_jobs:
            report += "## WARNING: the following jobs are still pending:\n"
            report += str([str(j) for j in self.future_jobs])
            report += str([str(j) for j in self.scheduled_jobs])

        # summarize all that junk
        njobs = len(self.future_jobs) + len(self.scheduled_jobs) +\
                len(self.completed_jobs)
        report += "\nnumber of jobs: " + str(njobs) + "\n"
        report += "wait time in seconds (completed jobs only):\n"
        if self.verbose:
            sys.stderr.write("completed jobs: " + str(self.completed_jobs) + "\n")
        wait_times = [j.start_time - j.arrival_time + 1 for j in self.completed_jobs]
        avg_wait = date_string_from_duration_in_seconds(sum(wait_times) / len(wait_times))
        max_wait = date_string_from_duration_in_seconds(max(wait_times))
        report += "\taverage: " + avg_wait + "\n"
        report += "\tmax: " + max_wait + "\n"
        report += "compute time in seconds (completed jobs only): \n"
        compute_times = [j.end_time - j.start_time + 1 for j in self.completed_jobs]
        avg_compute = date_string_from_duration_in_seconds(sum(compute_times) / len(compute_times))
        max_compute = date_string_from_duration_in_seconds(max(compute_times))
        report += "\taverage: " + avg_compute + "\n"
        report += "\tmax: " + max_compute + "\n"

        return report

    def generate_node_report(self):
        # name\ttotal_compute_time (sec)\ttotal_idle_time (sec)\t
        #        total_energy_consumption (kWh)
        report = ""
        for node in self.nodes.values():
            report += node.generate_report()
        return report
    
    def get_node_from_historical_node_name(self, hist_node):
        if hist_node == "compute-0-1.local":
            return self.nodes["slow1"]
        elif hist_node == "compute-0-2.local":
            return self.nodes["slow2"]
        elif hist_node == "compute-1-0.local":
            return self.nodes["fast"]
        else:
            return None

    def assign_job_from_historical_data(self, job):
        target_node = self.get_node_from_historical_node_name(job.historical_node)
        if not target_node:
            # TODO shouldn't this return false or something?
            if self.verbose:
                sys.stderr.write("unable to find corresponding node: " +
                                 job.historical_node + ". Skipping ...\n")
        job.start_time = job.historical_start_time
        job.end_time = job.historical_end_time
        job.node_name = target_node.name
        target_node.add_job(job)
        return True

    def assign_job_to_fast(self, job, newtime):
        target_node = self.nodes["fast"]
        if not target_node:
            if self.verbose:
                sys.stderr.write("unable to find fast node. Skipping job.\n")
            return False
        job.compute_time = calculate_compute_time(job, target_node)
        # can't ask for 48 cores on fast node
        if job.cpus_requested > target_node.cpus:
            # TODO <hack>
            job.cpus_requested = target_node.cpus
        if target_node.cpus_available(newtime) >= job.cpus_requested:
            job.start_time = newtime
            job.end_time = newtime + job.compute_time
            job.node_name = target_node.name
            target_node.add_job(job)
            return True
        else:
            if self.verbose:
                sys.stderr.write("unable to schedule job on fast node. Will try again later.\n")
            return False

    def assign_job(self, job, newtime):
        """Assigns job according to self.algorithm, returns True on success."""
        if self.algorithm == "historical":
            return self.assign_job_from_historical_data(job)
        elif self.algorithm == "allfast":
            return self.assign_job_to_fast(job, newtime)


