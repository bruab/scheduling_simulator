#!/usr/bin/env python
import sys
import datetime

def datetime_from_string(string):
    fields = string.split("_")
    if len(fields) != 3:
        sys.stderr.write("error parsing this date: " + string +\
                         " ... not enough fields.\n")
        return None
    month, day, year = int(fields[0]), int(fields[1]), int(fields[2])
    return datetime.datetime(year, month, day)


def  get_arrival_datetime(line):
    fields = line.strip().split(":")
    if len(fields) < 8:
        sys.stderr.write("error parsing this line: " + line +\
                         " ... not enough fields\n")
        return None
    arrival_epoch = int(fields[8])
    return datetime.datetime.fromtimestamp(arrival_epoch)

def main():

    if len(sys.argv) < 4:
        sys.stderr.write("usage: subset_job_submission_records_by_date_range.py <accounting file> <start_date> <end_date>\n")
        sys.stderr.write("dates should be in the format MM_DD_YYYY\n")
        sys.exit()

    # get start_date and end_date objects
    startstring = sys.argv[2]
    start_date = datetime_from_string(startstring)
    if not start_date:
        sys.stderr.write("failed to convert " + startstring +\
                         " to a date time, sorry. exiting.\n")

    endstring = sys.argv[3]
    end_date = datetime_from_string(endstring)
    if not end_date:
        sys.stderr.write("failed to convert " + endstring +\
                         " to a date time, sorry. exiting.\n")



    # open accounting file
    with open(sys.argv[1], 'r') as accfile:
        for line in accfile:
            # skip comments
            if line.startswith("#"):
                continue

            arrival_datetime = get_arrival_datetime(line)
            if not arrival_datetime:
                sys.stderr.write("failed to convert arrival datetime from " +\
                            "this line: " + line + " ... skipping ...\n")
                continue
            if arrival_datetime < start_date or arrival_datetime > end_date:
                sys.stderr.write("this line outside date range: " + line +\
                                 " ... skipping ...\n")
                continue
            else:
                sys.stdout.write(line)


###########################################

if __name__ == '__main__':
    main()
