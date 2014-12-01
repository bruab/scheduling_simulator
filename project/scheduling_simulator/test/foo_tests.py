#!/usr/bin/env python

import unittest
from src.foo import Foo

class TestFoo(unittest.TestCase):

    def setUp(self):
        self.foo = Foo()
        self.foo_with_bar = Foo(bar="dog")

    def test_echo(self):
        expected = "test string"
        actual = self.foo.echo("test string")
        self.assertEquals(expected, actual)

    def test_constructor_no_arg(self):
        self.assertFalse(self.foo.bar)
        
    def test_constructor_with_arg(self):
        self.assertEquals("dog", self.foo_with_bar.bar)

##########################
def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestFoo))
    return suite

if __name__ == '__main__':
    unittest.main()
