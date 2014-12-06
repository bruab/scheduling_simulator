#!/usr/bin/env python


class Scheduler:

    def __init__(self, nodes, jobs):
        self.nodes = nodes
        self.pending_jobs = jobs
        self.current_time = None
        self.scheduled_jobs = []
        self.completed_jobs = []

    def initialize(self, init_time):
        self.current_time = init_time
        # initialize nodes
        # assign any pending jobs that have arrived; move them to scheduled jobs

    def update(self, newtime):
        for node in self.nodes:
            # TODO
            # find out which jobs completed in the previous second (self.current_time)
            # move these jobs from scheduled to completed
            pass
        # find pending jobs that have arrived, assign and move them to scheduled jobs
        to_schedule = []
        for job in self.pending_jobs:
            if job.arrival_time <= newtime:
                to_schedule.append(job)
        for job in to_schedule:
            self.assign_job(job) # TODO what happens here?
            self.scheduled_jobs.append(job) # TODO dictionary instead? lots of removes
        # remove the jobs we just scheduled from the 'pending' list
        # (note doing it this way means at each tick all arrived jobs must be
        #  scheduled. iow no carrying them over and scheduling them in a few seconds)
        self.pending_jobs = [j for j in self.pending_jobs if j.arrival_time > newtime]
        # update time
        self.current_time = newtime

    def generate_job_report(self):
        #print("arrival_time\tstart_time\tcompletion_time\trun_time (seconds)\t")
        report = ""
        for job in self.completed_jobs:
            report += str(job)

        if self.pending_jobs or self.scheduled_jobs:
            report += "## WARNING: the following jobs are still pending:\n"
            report += str([str(j) for j in self.pending_jobs])
            report += str([str(j) for j in self.scheduled_jobs])

        return report

    def generate_node_report(self):
        #print("name\ttotal_compute_time (sec)\ttotal_idle_time (sec)\t" +
        #        "total_energy_consumption (kWh)")
        report = ""
        for node in self.nodes:
            report += node.generate_report()
        return report

    def assign_job(self, job):
        pass


