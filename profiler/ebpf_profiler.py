#!/usr/bin/env python3

import sys
import os
import re

file_path=sys.argv[1]
out_name=sys.argv[2]
ebpf_regions = 100

with open(file_path, 'r') as file:
    lines = file.readlines()

size_2MB = 2097152
line_count = len(lines)
workload_sz = line_count * size_2MB
regions = workload_sz / ebpf_regions

data_sum = 0
data_cnt = 0

inst_sum = 0
inst_cnt = 0

total_sum = 0
counter = 0

for line in lines:
    numbers = line.strip().split()

    start_address = numbers[0]
    freq = float(numbers[2])
    
    if start_address.startswith('0xa') or start_address.startswith('0x0') or len(start_address) < 14:
        inst_sum += freq
        inst_cnt += 1
    elif start_address.startswith('0xf'):
        data_sum += freq
        data_cnt += 1
    else:
        print(f"Couldn't classify address: {start_address} as ins/data")

    total_sum += freq
    counter += 1

avg_data = data_sum / data_cnt
avg_inst = inst_sum / inst_cnt
avg = total_sum / counter

data_cutoff = avg_data - avg_data*0.13
inst_cutoff = avg_inst - avg_inst*0.13

print(f"data average: {avg_data}, data cutoff: {data_cutoff}")
print(f"inst average: {avg_inst}, isnt cuttof: {inst_cutoff}")
print(f"total: {avg}")

# Instructions region
contig = False

tmp_inst_addr_start = int((lines[0].strip().split())[0], 0)
tmp_inst_addr_end = 0
print(f"0x{tmp_inst_addr_start:x}")

tmp_data_addr_start = int((lines[inst_cnt].strip().split())[0], 0)
tmp_data_addr_end = 0
print(f"0x{tmp_data_addr_start:x}")

out_file = open(out_name, 'w')

for line in lines[0:inst_cnt]:
    numbers = line.strip().split()

    start_address = int(numbers[0], 0)
    end_address = int(numbers[1], 0)
    freq = float(numbers[2])

    if freq < inst_cutoff:
        if contig:
            out_file.write(f"0x{tmp_inst_addr_start:x},0x{tmp_inst_addr_end:x},1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2\n")
            contig = False
        tmp_inst_addr_start = end_address
    else:
        tmp_inst_addr_end = end_address
        contig = True

if contig:
    out_file.write(f"0x{tmp_inst_addr_start:x},0x{tmp_inst_addr_end:x},1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2\n")

# Data region
contig = False

for line in lines[inst_cnt:]:
    numbers = line.strip().split()
    
    start_address = int(numbers[0], 0)
    end_address = int(numbers[1], 0)
    freq = float(numbers[2])

    if freq < data_cutoff:
        if contig:
            out_file.write(f"0x{tmp_data_addr_start:x},0x{tmp_data_addr_end:x},1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2\n")
            contig = False
        tmp_data_addr_start = end_address
    else:
        tmp_data_addr_end = end_address
        contig = True

if contig:
    out_file.write(f"0x{tmp_data_addr_start:x},0x{tmp_data_addr_end:x},1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2\n")

print("Done.")


