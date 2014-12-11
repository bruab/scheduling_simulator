#!/usr/bin/env python


class Job:

    def __init__(self, cpus, arrival_time, historical_start_time, historical_end_time, historical_node):
        self.cpus_requested = cpus
        self.arrival_time = arrival_time
        self.historical_node = historical_node
        self.historical_start_time = historical_start_time
        self.historical_end_time = historical_end_time
        self.node_name = None
        self.start_time = None
        self.end_time = None
        self.compute_time = None

    def __str__(self):
        result = "Job object:\n"
        result += "\tarrival/start/end: " + str(self.arrival_time) + "/"
        result += str(self.start_time) + "/" + str(self.end_time) + "\n"
        result += "\tcpus requested: " + str(self.cpus_requested) + "\n"
        result += "\tcompute_time: " + str(self.compute_time) + "\n"
        return result
