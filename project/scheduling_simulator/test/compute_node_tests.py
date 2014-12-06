#!/usr/bin/env python

import unittest
from src.compute_node import ComputeNode

class TestComputeNode(unittest.TestCase):

    def setUp(self):
        self.node1 = ComputeNode("node1", 400, 300)
    
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
        

##########################
def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestComputeNode))
    return suite

if __name__ == '__main__':
    unittest.main()
