#!/usr/bin/env python3

import sys
import os
import re

file_path=sys.argv[1]
out_name=sys.argv[2]
ebpf_profile_size=int(sys.argv[3])

with open(file_path, 'r') as file:
    lines = file.readlines()

regions = []
for line in lines:
    numbers = line.strip().split()

    start_address = numbers[0]
    end_address = numbers[1]
    freq = float(numbers[2])

    obj = (start_address, end_address, freq)
    regions.append(obj)

sorted_regions = sorted(regions, key=lambda x: x[2], reverse=True)

out_file = open(out_name, 'w')

count = 0
for item in sorted_regions:
    print(item)

    if count < ebpf_profile_size:
        out_file.write(f"{item[0]},{item[1]},1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,2\n")

    count += 1

