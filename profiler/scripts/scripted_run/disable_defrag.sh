#!/bin/bash

proc_path="/proc/sys/vm"
khugepaged_path="/sys/kernel/mm/transparent_hugepage/khugepaged"

echo 1000 > "${proc_path}/extfrag_threshold"
echo 0 > "${proc_path}/compaction_proactiveness"

echo 1 > "${khugepaged_path}/pages_to_scan"
echo $(( 3600 * 1000 ))> "${khugepaged_path}/scan_sleep_millisecs"
echo $(( 3600 * 1000 )) > "${khugepaged_path}/alloc_sleep_millisecs"

echo 0 > "${khugepaged_path}/defrag"
