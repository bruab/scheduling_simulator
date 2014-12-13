#!/usr/bin/env python

import unittest
from unittest.mock import Mock
from src.compute_node import ComputeNode

class TestComputeNode(unittest.TestCase):

    def setUp(self):
        self.node1 = ComputeNode("node1", 400, 300, cpus=36)
    
    def test_calculate_total_compute_time(self):
        self.node1.compute_times = [(10, 20), (8, 12), (4, 6)]
        expected = 14
        actual = self.node1.calculate_total_compute_time()
        self.assertEquals(expected, actual)

    def test_calculate_total_compute_time_with_redundancy(self):
        self.node1.compute_times = \
                [(20, 25), (10, 15), (10, 15), (5, 15)]
        expected = 15
        actual = self.node1.calculate_total_compute_time()
        self.assertEquals(expected, actual)

    def test_find_job_start_time(self):
        # node has 36 cpus
        # newjob arrives at time 100
        # job1 runs from 90 to 110, takes 6 cores
        # job2 runs from 80 to 120, takes 6 cores
        # newjob wants 30 cores, so can't run at 100
        # but at time 111, when job1 is complete, can start
    #def __init__(self, cpus, arrival_time, historical_start_time, historical_end_time, historical_node):
    #    self.cpus_requested = cpus
        self.node1.current_time = 100
        job1 = Mock()
        job1.cpus_requested = 6
        job1.start_time = 90
        job1.end_time = 110
        job2 = Mock()
        job2.cpus_requested = 6
        job2.start_time = 80
        job2.end_time = 120
        self.node1.current_jobs = [job1, job2]
        newjob = Mock()
        newjob.arrival_time = 100
        newjob.cpus_requested = 30
        newjob.compute_time = 200
        expected = 111
        actual = self.node1.find_job_start_time(newjob)
        self.assertEquals(expected, actual)
        

##########################
def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestComputeNode))
    return suite

if __name__ == '__main__':
    unittest.main()
