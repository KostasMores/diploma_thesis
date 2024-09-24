#!/bin/bash

echo 0 > /proc/sys/kernel/randomize_va_space
sync
echo 3 > /proc/sys/vm/drop_caches

echo never >/sys/kernel/mm/transparent_hugepage/hugepages-16kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-32kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-64kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-128kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-256kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-512kB/enabled
echo never >/sys/kernel/mm/transparent_hugepage/hugepages-1024kB/enabled
echo always >/sys/kernel/mm/transparent_hugepage/hugepages-2048kB/enabled

echo "ASLR disabled, caches dropped, synced with disk"
