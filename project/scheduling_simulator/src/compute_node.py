#!/usr/bin/env python

from src.util import date_string_from_duration_in_seconds, date_string_from_epoch_timestamp

def calculate_compute_time(job, node):
    """Given a Job and ComputeNode, determine how long the job will compute.

    Use DISCOVAR benchmark data.
    """
    # TODO
    return job.historical_end_time - job.historical_start_time
class ComputeNode: 
    def __init__(self, name, running_watts, idle_watts, cpus, verbose=False):
        self.name = name
        self.running_watts = running_watts
        self.idle_watts = idle_watts
        self.cpus = cpus
        self.verbose = verbose
        self.compute_times = []
        self.idle_times = []
        self.current_jobs = []
        self.compute_time = 0
        self.idle_time = 0
        self.current_time = None

    def was_running(self, time):
        for job in self.current_jobs:
            if job.start_time <= time and job.end_time >= time:
                return True
        return False

    def cpus_in_use(self, time):
        count = 0
        for job in self.current_jobs:
            if job.start_time <= time and job.end_time >= time:
                count += job.cpus_requested
        return count

    def cpus_available(self, time):
        in_use = self.cpus_in_use(time)
        return self.cpus - in_use

    def x_cpus_available_for_y_seconds(self, cpus_requested, seconds, start_time):
        for time in range(start_time, start_time+seconds+1):
            cpus_available = self.cpus - self.cpus_in_use(time)
            if cpus_available < cpus_requested:
                return False
        return True

    def initialize(self, time):
        self.current_time = time

    def update(self, newtime):
        """Returns a list of jobs completed in the period previous to newtime.

        Also increments self.compute_time or self.idle_time.
        """
        completed_jobs = []
        for job in self.current_jobs:
            if job.end_time < newtime:
                completed_jobs.append(job)
                # remove this job from self.current_jobs
                self.current_jobs = [j for j in self.current_jobs if j not in completed_jobs]
        # increment running/idle time
        if self.was_running(newtime-1):
            self.compute_time += 1
        else:
            self.idle_time += 1
        self.current_time = newtime
        return completed_jobs

    def calculate_total_compute_time(self):
        self.compute_times = sorted(self.compute_times)
        wall_times = []
        current_start = None
        current_end = None
        for time_tuple in self.compute_times:
            if not current_start:
                current_start = time_tuple[0]
                current_end = time_tuple[1]
                continue

            if time_tuple[0] > current_end:
                wall_times.append( (current_start, current_end) )
                current_start = time_tuple[0]
                current_end = time_tuple[1]
            elif time_tuple[0] == current_end:
                current_end = time_tuple[1]
            elif time_tuple[0] < current_end:
                if time_tuple[1] > current_end:
                    current_end = time_tuple[1]
        wall_times.append( (current_start, current_end) )
        total_compute_time = sum([t[1] - t[0] for t in wall_times])
        return total_compute_time

    def generate_report(self):
        # name\ttotal_compute_time\ttotal_idle_time\t
        #        total_energy_consumption (kWh)
        running_cost = (self.compute_time * self.running_watts / 3600) / 1000
        idle_cost = (self.idle_time * self.idle_watts / 3600) / 1000
        energy_cost = running_cost + idle_cost
        compute_time = date_string_from_duration_in_seconds(self.compute_time)
        idle_time = date_string_from_duration_in_seconds(self.idle_time)
        data = [self.name, compute_time, idle_time, str(energy_cost)]
        return "\t".join(data) + "\n"

    def add_job(self, job):
        self.current_jobs.append(job)

    def find_job_start_time(self, job):
        """Given a job, find the earliest start time available.

        Consider the cpus requested for the job in question as well as
          any running and scheduled jobs.

        Return the time in epoch seconds.
        """
        TIME_LIMIT = 1000 # only check for 1000 sec after job arrival
        time = self.current_time
        job_start_time = None
        if self.verbose:
            sys.stderr.write("find_job_start_time here, looking for a time to start job " + str(job))
            sys.stderr.write("currently have " + str(len(self.current_jobs)) + " jobs...")
            sys.stderr.write("and using " + str(self.cpus_in_use(time)) + " at time " + str(time))
        while not job_start_time:
            if self.verbose and time % 10 == 0:
                sys.stderr.write("\tnow checking time " + str(time))
            if time == job.arrival_time + TIME_LIMIT:
                return None
            cpus_available = self.cpus - self.cpus_in_use(time)
            if cpus_available >= job.cpus_requested:
                if self.x_cpus_available_for_y_seconds(job.cpus_requested, job.compute_time, time):
                    if self.verbose:
                        sys.stderr.write("\tfound it! the job can run at time " + str(time) + "\n\n")
                    return time
            time += 1
