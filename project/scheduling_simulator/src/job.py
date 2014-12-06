#!/usr/bin/env python


class Job:

    def __init__(self, arrival_time, start_time, end_time, target_node=None):
        self.arrival_time = arrival_time
        self.start_time = start_time
        self.end_time = end_time
        self.target_node = target_node

    def echo(self, input):
        return input

