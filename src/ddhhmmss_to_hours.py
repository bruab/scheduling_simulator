#!/usr/bin/env python

import sys
import datetime

def ddhhmmss_to_hours(s):
    fields = s.split(":")
    days = int(fields[0])
    hours = int(fields[1])
    minutes = int(fields[2])
    seconds = int(fields[3])
    total_hours = 24*days + hours + (minutes/(24*60)) + (seconds/(24*60*60))
    return total_hours

def main():

    instring = sys.argv[1]
    print(ddhhmmss_to_hours(instring))


###########################################

if __name__ == '__main__':
    main()
