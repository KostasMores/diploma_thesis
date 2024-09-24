#!/usr/bin/env python3

import sys
import os
import re

file_path=sys.argv[1]

with open(file_path, 'r') as file:
    lines = file.readlines()

total_sum = 0
counter = 0

for line in lines:
    numbers = line.strip().split()

    freq = float(numbers[2])

    total_sum += freq
    counter += 1

avg = total_sum / counter
match = re.search(r'hashjoin_(\d+)\.txt', os.path.basename(file_path))

if not match:
    sys.exit(f"Wrong filename provided: {file_path}")

size_2MB = 2097152
addr_start = int(match.group(1))
addr_end = addr_start + size_2MB

print(f"0x{addr_start:x} 0x{addr_end:x}", end=" ")
print(avg)

