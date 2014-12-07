#!/usr/bin/env python

import sys
from src.util import date_string_from_epoch_timestamp, date_string_from_duration_in_seconds

class Scheduler:

    def __init__(self, nodes, jobs):
        self.nodes = {}
        for node in nodes:
            self.nodes[node.name] = node
        self.pending_jobs = []
        for job in jobs:
            # skip jobs that ran on nodes we don't know about
            if not self.get_node_from_historical_node_name(job.historical_node):
                continue
            self.pending_jobs.append(job)
        self.current_time = None
        self.scheduled_jobs = []
        self.completed_jobs = []

    def initialize(self, init_time):
        self.current_time = init_time
        for node in self.nodes.values():
            node.initialize(init_time)

    def update(self, newtime):
        for node in self.nodes.values():
            completed_jobs = node.update(newtime)
            self.completed_jobs += completed_jobs
            self.scheduled_jobs = [j for j in self.scheduled_jobs if j not in completed_jobs]
        # find pending jobs that have arrived, assign and move them to scheduled jobs
        to_schedule = []
        for job in self.pending_jobs:
            if job.arrival_time <= newtime:
                to_schedule.append(job)
        for job in to_schedule:
            self.assign_job(job) 
            self.scheduled_jobs.append(job) 
        # remove the jobs we just scheduled from the 'pending' list
        # (note doing it this way means at each tick all arrived jobs must be
        #  scheduled. iow no carrying them over and scheduling them in a few seconds)
        self.pending_jobs = [j for j in self.pending_jobs if j.arrival_time > newtime]
        # update time
        self.current_time = newtime

    def get_next_job_arrival_time(self):
        if not self.pending_jobs:
            return None
        else:
            return self.pending_jobs[0].arrival_time

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

        if self.pending_jobs or self.scheduled_jobs:
            report += "## WARNING: the following jobs are still pending:\n"
            report += str([str(j) for j in self.pending_jobs])
            report += str([str(j) for j in self.scheduled_jobs])

        # summarize all that junk
        njobs = len(self.pending_jobs) + len(self.scheduled_jobs) +\
                len(self.completed_jobs)
        report += "\nnumber of jobs: " + str(njobs) + "\n"
        report += "average wait time in seconds (completed jobs only): "
        wait_times = [j.start_time - j.arrival_time for j in self.completed_jobs]
        avg_wait = sum(wait_times) / len(wait_times)
        report += str(avg_wait) + "\n"

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
            sys.stderr.write("unable to find corresponding node: " +
                             job.historical_node + ". Skipping ...\n")
            # TODO now what.
        job.start_time = job.historical_start_time
        job.end_time = job.historical_end_time
        job.node_name = target_node.name
        target_node.add_job(job)


    def assign_job(self, job):
        # for now just use historical data to do this
        self.assign_job_from_historical_data(job)


