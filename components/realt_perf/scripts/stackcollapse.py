#!/usr/bin/env python3
#
# Copyright (c) 2023 KNS Group LLC (YADRO)
# Copyright (c) 2020 Yonatan Goldschmidt <yon.goldschmidt@gmail.com>
#
# SPDX-License-Identifier: Apache-2.0

"""
Stack compressor for FlameGraph

This translate stack samples captured by perf subsystem into format
used by flamegraph.pl. Translation uses .elf file to get function names
from addresses

Usage:
    python3 stackcollapse.py <file with perf printbuf output> <ELF file>
"""

import re
import sys
import struct
import binascii
from functools import lru_cache
from elftools.elf.elffile import ELFFile
from prettytable import PrettyTable

# typedef struct {
# #define PERF_DATA_HEAD_MAGIC 0x434F5950
# 	uint32_t magic;
# 	uint32_t duration_ms;
# 	uint32_t frequency;
# 	uint16_t cpu;
# 	uint16_t rsv;
# 	uint32_t elapsed_ms;
# 	uint32_t rsv2[11];
# 	unsigned long buf[];
# } perf_buf_head_t;
perfdata_head_size=64
perfdata_head_magic=0x434F5950
long_type_size=int(8)
class PerfData:
    def __init__(self):
        self.magic = 0
        self.duration_ms = 0
        self.frequency = 0
        self.cpu = 0
        self.elapsed_ms = 0

debug_mode=False
func_call_arrow=' <- '

@lru_cache(maxsize=None)
def addr_to_sym(addr, elf):
    symtab = elf.get_section_by_name(".symtab")
    for sym in symtab.iter_symbols():
        if sym.entry.st_info.type == "STT_FUNC" and sym.entry.st_value <= addr < sym.entry.st_value + sym.entry.st_size:
            return sym.name
    if addr == 0:
        return "nullptr"
    return hex(addr) #"[unknown]"

def debug_print(*args):
    if debug_mode:
        print(*args)

def check_head(perf_data, buf):
    buf_head = buf[:perfdata_head_size]
    perf_data.magic, = struct.unpack_from("<I", buf_head)
    if perf_data.magic != perfdata_head_magic:
        print("the perf rtos data error.")
        exit(1)
    perf_data.duration_ms,  = struct.unpack_from("<I", buf_head[4:])
    perf_data.frequency,  = struct.unpack_from("<I", buf_head[8:])
    perf_data.cpu,  = struct.unpack_from("<H", buf_head[12:])
    perf_data.elapsed_ms,  = struct.unpack_from("<I", buf_head[16:])

    debug_print("perf_data.duration_ms: ", perf_data.duration_ms)
    debug_print("perf_data.frequency: ", perf_data.frequency)
    debug_print("perf_data.cpu: ", perf_data.cpu)
    debug_print("perf_data.elapsed_ms: ", perf_data.elapsed_ms)
    return buf[perfdata_head_size:]

def fold(perf_data, buf, elf):
    trace_list = []
    while buf:
        if long_type_size == 8:
            count, = struct.unpack_from("<Q", buf)
        else:
            count, = struct.unpack_from("<I", buf)
        if count == 0:
            break
        if long_type_size == 8:
            addrs = struct.unpack_from(f"<{count}Q", buf, long_type_size)
        else:
            addrs = struct.unpack_from(f"<{count}I", buf, long_type_size)
        func_trace = list(map(lambda a: addr_to_sym(a, elf), addrs))
        # Reverse the function trace to have the root function first
        func_trace = func_trace[::-1]
        # Join function names with ';' to create the stack trace
        line_trace = ';'.join(func_trace)
        trace_list.append(line_trace)
        buf = buf[long_type_size + long_type_size * count:]

    # Count the occurrences of each unique stack trace
    trace_dict = {}
    for line_trace in trace_list:
        if line_trace in trace_dict:
            trace_dict[line_trace] += 1
        else:
            trace_dict[line_trace] = 1

    # Output the stack traces in a format compatible with flamegraph.pl
    with open('%d.fold' % perf_data.cpu, 'w') as f:
        for trace, count in trace_dict.items():
            f.write(f'{trace} {count}\n')

def collapse(perf_data, buf, elf):
    record_count = 0
    trace_list = []
    while buf:
        if long_type_size == 8:
            count, = struct.unpack_from("<Q", buf)
        else:
            count, = struct.unpack_from("<I", buf)
        debug_print("\ncount: ", count)
        if count == 0:
            break
        if long_type_size == 8:
            addrs = struct.unpack_from(f"<{count}Q", buf, long_type_size)
        else:
            addrs = struct.unpack_from(f"<{count}I", buf, long_type_size)
        # for ad in addrs:
        #     debug_print("ad:", hex(ad))
        func_trace = list(map(lambda a: addr_to_sym(a, elf), addrs))
        # debug_print(func_trace)
        line_trace = ''
        if len(func_trace) > 0:
            for l in func_trace:
                line_trace += l + func_call_arrow
            line_trace = line_trace[: len(line_trace) - len(func_call_arrow)] # rm last ;
        trace_list.append(line_trace)
        buf = buf[long_type_size + long_type_size * count:]
        record_count = record_count + 1
    # debug_print("record_count: ", record_count)
    # debug_print(trace_list)
    trace_dict = {}
    for line_trace in trace_list:
        if line_trace in trace_dict:
            var = trace_dict[line_trace]
            var = var + 1
            trace_dict[line_trace] = var
        else:
            trace_dict[line_trace] = 1
    trace_dict2 = sorted(trace_dict.items(), key=lambda x:x[1], reverse=True)
    debug_print(trace_dict2)
    curpc_list = []     # [['func0', 'backtrace', count], [...], ...]
    for c in trace_dict2:
        c = list(c)
        func0 = c[0].split(func_call_arrow)[0]
        c.insert(0, func0)
        curpc_list.append(c)
    debug_print(curpc_list)
    merge_func0_dict = {} # {'func0': 'backtrace:count;backtrace2:count2', 'func1': '...'}
    for c in curpc_list:
        if c[0] not in merge_func0_dict:
            merge_func0_dict[c[0]] = '{}:{}'.format(c[1], c[2])
        else:
            value = merge_func0_dict[c[0]]
            value = value + ";" + '{}:{}'.format(c[1], c[2])
            merge_func0_dict[c[0]] = value
    debug_print(merge_func0_dict)
    merge_func0_list = []
    for key, value in merge_func0_dict.items():
        aval =  value.split(';')
        bval = []
        cnt = 0
        for a in aval:
            aa = a.split(':')
            cnt += int(aa[1])
            bval.append(aa)
        ele = [key, bval, cnt]
        merge_func0_list.append(ele)
    merge_func0_list = sorted(merge_func0_list, key=lambda x:x[2], reverse=True)
    debug_print(merge_func0_list)

    # show information
    heads_info = "Samples: {}; Frequency: {}Hz; Duration: {}ms; Elapsed: {}ms".format(record_count, perf_data.frequency, perf_data.duration_ms, perf_data.elapsed_ms)
    print(heads_info)
    table = PrettyTable(['CPU', 'Num Sample','Percent', 'Symbol', 'Call-Graph'])
    table.align['Symbol'] = 'l'
    table.align['Call-Graph'] = 'l'
    for func0_list in merge_func0_list:
        sample_percent = '{:.2%}'.format(func0_list[2] / record_count)
        nameline = ''
        for n in func0_list[1]:
            nameline += '[{:.2%}] {}\n'.format(int(n[1]) / func0_list[2], n[0])
        nameline = nameline[:-1]
        # symbol = '{}\n{}'.format(func0_list[0], nameline)
        table.add_row([perf_data.cpu, func0_list[2], sample_percent, func0_list[0], nameline], divider=True)
    print(table)

if __name__ == "__main__":
    if len(sys.argv) < 3 or not sys.argv[1] or not sys.argv[2]:
        print("please check the input arguments.")
        print("python3 stackcollapse.py <file with perf_rtos.data> <ELF file>")
        exit(1)
    if sys.argv[-1] == '-d':
        debug_mode = True

    elf = ELFFile(open(sys.argv[2], "rb"))
    if not elf:
        print('This is not a valid ELF file.')
        exit(1)
    header = elf.header
    if header.e_ident['EI_CLASS'] == 'ELFCLASS32':
        long_type_size = int(4)
    with open(sys.argv[1], "rb") as f:
        inp = f.read()
    # debug_print(inp)
    buf = inp
    perf_data = PerfData()
    buf = check_head(perf_data, buf)
    collapse(perf_data, buf, elf)
    fold(perf_data, buf, elf)
