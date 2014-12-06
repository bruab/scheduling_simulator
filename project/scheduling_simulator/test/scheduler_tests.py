#!/usr/bin/env python

import unittest
from src.scheduler import Scheduler

class TestScheduler(unittest.TestCase):

    def setUp(self):
        self.scheduler = Scheduler()
    
        

##########################
def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestScheduler))
    return suite

if __name__ == '__main__':
    unittest.main()
