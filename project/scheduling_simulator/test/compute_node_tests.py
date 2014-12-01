#!/usr/bin/env python

import unittest
from src.compute_node import ComputeNode

class TestComputeNode(unittest.TestCase):

    def setUp(self):
        pass

    def test_echo(self):
        pass

##########################
def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestComputeNode))
    return suite

if __name__ == '__main__':
    unittest.main()
