#!/bin/bash

MEASUREMENTS="cycles,instructions,cpu_cycles,minor-faults,major-faults,page-faults,l1d_tlb,l2d_tlb,dtlb_walk,armv8_pmuv3/dTLB-loads/,armv8_pmuv3/dTLB-load-misses/,l1i_tlb,itlb_walk,armv8_pmuv3/iTLB-loads/,armv8_pmuv3/iTLB-load-misses/"

if [ $# -ne 2 ]; then
        echo "Usage: ${0} [workload] [output_file]"
        exit 1
fi

workload=${1}
out_file=${2}

start_file="/tmp/XSBench.start"
alloc_file="/tmp/XSBench_alloc.ready"

out_path="/root/final_data/xsbench"
pid=""

khugepaged_before=$(cat /sys/kernel/mm/transparent_hugepage/khugepaged/pages_collapsed)
echo "Waiting to start XSBench..."
while [ ! -f "${start_file}" ]; do
        sleep 0.1       
done
echo "XSBench Started."

while true; do
        pid=$(pidof ${workload})
        if [ -n "$pid" ]; then
                echo "$pid"
                break
        fi
        sleep 0.1
done

echo "Measuring initilization and allocation"
./pflat.sh "start" ${workload}
perf stat -e ${MEASUREMENTS} -o ${out_path}/${out_file}_alloc.stats --pid=${pid}
./pflat.sh "stop"
./pflat.sh "show" > "${out_path}/${out_file}_alloc.hist"
./pflat.sh "clear"

echo "Waiting for workload..."

khugepaged_after=$(cat /sys/kernel/mm/transparent_hugepage/khugepaged/pages_collapsed)
khugepaged_pages_collapsed=$((khugepaged_after - khugepaged_before))
echo "khugepaged collapsed pages: ${khugepaged_pages_collapsed}"
echo "Workload done."

echo "Removing created files"

if [ -f "${start_file}" ]; then
	rm ${start_file}
fi

if [ -f "${alloc_file}" ]; then
        rm ${alloc_file}
fi

