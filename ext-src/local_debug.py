# -*- coding: utf-8 -*-

"""
Created on 2017-12-27 12:46
@summary: XAPM Extension Log Parse Agent
@author: tanhp@outlook.com
"""

import json

LOG_FILE = "/tmp/trace.log"


def parse_log(path):
    """
    parse trace log
    :param path:
    :return:
    """
    trace_list = []
    trace_one = []
    with open(path) as file:
        while True:
            line = file.readline()
            if not line:
                break
            line = line.strip("\n")
            if not line:
                continue
            info = line.split("\t")
            if len(info) == 2:
                if info[1] == "xapm_trace_start":
                    trace_one = []
                elif info[1] == "xapm_trace_end":
                    trace_list.append(map_trace(trace_one))
                else:
                    if info[1].isdigit():
                        trace_one.append(info[1])
                    else:
                        trace_one.append(line)
    return trace_list


def map_trace(trace):
    """
    map trace
    :param trace:
    :return:
    """
    sorts = []
    stack = []
    result = []
    for idx, val in enumerate(trace):
        if not val.isdigit():
            stack.append(json.dumps({val: idx}))
        else:
            sorts.append({
                stack.pop(): int(val)
            })
    sorts.sort(cmp=cmp_trace)
    for elem in sorts:
        key = elem.keys()[0]
        val = elem.values()[0]
        result.append(split_detail(json.loads(key).keys()[0]) + "\t" + str(val) + "ms")
    return result


def split_detail(detail):
    """
    split detail
    :param detail:
    :return:
    """
    info_list = detail.split("*")
    if len(info_list) >= 2:
        return "\t".join(info_list)
    return ""


def cmp_trace(x, y):
    """
    compare trace
    :param x:
    :param y:
    :return:
    """
    jx = x.keys()[0]
    jy = y.keys()[0]
    return json.loads(jx).values()[0] - json.loads(jy).values()[0]


if __name__ == "__main__":
    traces = parse_log(LOG_FILE)
    for elem in traces:
        print "xapm_trace_start"
        for tmp in elem:
            print tmp
        print "xapm_trace_end\n"
