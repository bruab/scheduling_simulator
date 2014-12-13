#!/usr/bin/env python

import unittest
from src.job import Job

class TestJob(unittest.TestCase):

    def setUp(self):
        pass

    def test_echo(self):
        pass

##########################
def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestJob))
    return suite

if __name__ == '__main__':
    unittest.main()
