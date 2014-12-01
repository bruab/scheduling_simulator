#!/usr/bin/env python


class Job:

    def __init__(self, arrival_time, compute_time, target_node=None):
        self.arrival_time = arrival_time
        self.compute_time = compute_time
        self.target_node = target_node

    def echo(self, input):
        return input

