#!/usr/bin/env python3

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

