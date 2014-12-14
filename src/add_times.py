#!/usr/bin/env python

import sys
import datetime

def timedelta_from_string(s):
    fields = s.split(":")
    days = int(fields[0])
    hours = int(fields[1])
    minutes = int(fields[2])
    seconds = int(fields[3])
    total_seconds = hours*3600 + minutes*60 + seconds
    return datetime.timedelta(days, total_seconds)

def main():

    sum = datetime.timedelta()
    for time in sys.argv[1:]:
        sum += timedelta_from_string(time)
    print(sum)


###########################################

if __name__ == '__main__':
    main()
