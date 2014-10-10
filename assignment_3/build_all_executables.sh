#!/bin/bash

smpicc question1.c -DVERSION=default -o question1
smpicc question1.c -DVERSION=naive -o question1_naive
