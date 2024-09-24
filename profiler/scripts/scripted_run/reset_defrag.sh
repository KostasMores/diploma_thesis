#!/bin/bash

proc_path="/proc/sys/vm"
khugepaged_path="/sys/kernel/mm/transparent_hugepage/khugepaged"

echo 500 > "${proc_path}/extfrag_threshold"
echo 20 > "${proc_path}/compaction_proactiveness"

echo 4096 > "${khugepaged_path}/pages_to_scan"
echo 10000 > "${khugepaged_path}/scan_sleep_millisecs"
echo 60000 > "${khugepaged_path}/alloc_sleep_millisecs"

echo 1 > "${khugepaged_path}/defrag"
