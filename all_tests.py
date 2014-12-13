#!/usr/bin/env python

# import all the lovely files
import unittest
import test.compute_node_tests
import test.job_tests

# get suites from test modules
suites = [
test.compute_node_tests.suite(),
test.job_tests.suite(),
]

# collect suites in a TestSuite object
suite = unittest.TestSuite()
for s in suites:
    suite.addTest(s)

# run suite
unittest.TextTestRunner(verbosity=2).run(suite)
