#!/bin/bash

KHUGEPAGED_CTRL="/sys/kernel/mm/transparent_hugepage/khugepaged"

echo "Disabling khugepaged..."
echo 0 >${KHUGEPAGED_CTRL}/defrag
echo 0 >${KHUGEPAGED_CTRL}/pages_to_scan
