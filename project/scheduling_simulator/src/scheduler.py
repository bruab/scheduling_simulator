#!/usr/bin/env python


class Scheduler:

    def __init__(self, nodes, jobs):
        self.nodes = nodes
        self.pending_jobs = jobs
        self.current_time = None
        self.scheduled_jobs = []

    def initialize(self, init_time):
        self.current_time = init_time

    def update(self, newtime):
        pass

    def generate_job_report(self):
        #print("arrival_time\tstart_time\tcompletion_time\trun_time (seconds)\t")
        report = ""
        for job in self.scheduled_jobs:
            report += str(job)

        if self.pending_jobs:
            report += "## WARNING: the following jobs are still pending:\n"
            report += str([str(j) for j in self.pending_jobs])

        return report

    def generate_node_report(self):
        #print("name\ttotal_compute_time (sec)\ttotal_idle_time (sec)\t" +
        #        "total_energy_consumption (kWh)")
        report = ""
        for node in self.nodes:
            report += node.generate_report()
        return report
