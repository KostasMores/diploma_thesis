#!/bin/bash

echo 0 > /proc/sys/kernel/randomize_va_space
sync
echo 3 > /proc/sys/vm/drop_caches

echo "ASLR disabled, synced with disk, caches dropped"
