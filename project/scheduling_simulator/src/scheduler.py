#!/usr/bin/env python


class Scheduler:

    def __init__(self, nodes, jobs):
        self.nodes = nodes
        self.jobs = jobs

    def initialize(self, init_time):
        pass

    def update(self, newtime):
        pass

    def generate_job_report(self):
        return "job report"

    def generate_node_report(self):
        return "node report"
