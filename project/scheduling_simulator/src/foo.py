#!/usr/bin/env python


class Foo:

    def __init__(self, bar=None):
        self.bar = bar

    def echo(self, input):
        return input

    def get_bar(self):
        return self.bar
