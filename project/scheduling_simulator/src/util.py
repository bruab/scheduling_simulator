#!/usr/bin/env python3

from src.job import Job
import datetime

def date_string_from_epoch_timestamp(epoch):
    result = ""
    date_time = datetime.datetime.fromtimestamp(epoch)
    m_d_y = [date_time.month, date_time.day, date_time.year]
    m_d_y = [str(x) for x in m_d_y]
    result += "/".join(m_d_y)
    h_m_s = [date_time.hour, date_time.minute, date_time.second]
    h_m_s = [str(x) for x in h_m_s]
    result += " " + ":".join(h_m_s)
    return result

def date_string_from_duration_in_seconds(seconds):
    result = ""
    td = datetime.timedelta(seconds=seconds)
    d = datetime.datetime(1,1,1) + td
    result += str(d.day-1) + ":"
    result += str(d.hour) + ":"
    result += str(d.minute) + ":"
    result += str(d.second)
    return result

def jobs_from_accounting_file(accounting_file):
    jobs = []
    with open(accounting_file, 'r') as accfile:
        for line in accfile:
            # sample line:
            # all.q:compute-0-1.local:users:sgeib:assembly.qsub:2:sge:0:1395639825:1395639831:1395639831:0:0:0:0.107983:0.217966:2700.000000:0:0:0:0:26527:0:0:8.000000:24:0:0:0:226:157:NONE:defaultdepartment:orte:32:0:0.325949:0.000000:0.000000:-pe orte 32:0.000000:NONE:0.000000:0:0
            if line.startswith("#"):
                # header or comment
                continue

            if not ":" in line:
                # header or blank or error or something
                continue

            fields = line.strip().split(":")
            if len(fields) < 10:
                # something is wrong
                sys.stderr.write("couldn't figure out how to parse this line, skipped it: " + line)
                continue

            # get run time, target node, cpus
            target_node = fields[1]
            arrival_time = int(fields[8])
            start_time = int(fields[9])
            end_time = int(fields[10])
            run_time = int(end_time) - int(start_time)
            cpus = int(fields[34])

            if not start_time: # job never ran
                continue

            # create Job and add it to list
            job = Job(cpus, arrival_time, start_time, end_time, target_node)
            jobs.append(job)
    return jobs

