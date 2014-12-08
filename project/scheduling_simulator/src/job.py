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

