#!/usr/bin/env python


class ComputeNode:

    def __init__(self, name, watts_per_second, speed_factor=1.0):
        self.name = name
        self.watts_per_second = watts_per_second
        self.speed_factor = speed_factor
        self.wattages = []
        self.compute_times = []
        self.idle_times = []

    def echo(self, input):
        return input

