#!/usr/bin/env python3

import sys
import os
import re

file_path=sys.argv[1]
out_name=sys.argv[2]

with open(file_path, 'r') as file:
    lines = file.readlines()

regions = []
for line in lines:
    numbers = line.strip().split()

    start_address = int(numbers[0], 0)
    end_address = numbers[1]
    freq = float(numbers[2])

    obj = (start_address, end_address, freq)
    regions.append(obj)

sorted_regions = sorted(regions, key=lambda x: x[0], reverse=False)

out_file = open(out_name, 'w')

for item in sorted_regions:
    print(item)

    out_file.write(f"0x{item[0]:x} {item[1]} {item[2]}\n")


