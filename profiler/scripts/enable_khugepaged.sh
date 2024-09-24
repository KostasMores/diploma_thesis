#!/bin/bash

KHUGEPAGED_CTRL="/sys/kernel/mm/transparent_hugepage/khugepaged"

echo "Enabling khugepaged..."
echo 1 > ${KHUGEPAGED_CTRL}/defrag
echo 4096 > ${KHUGEPAGED_CTRL}/pages_to_scan
