#!/bin/bash

MEASUREMENTS="cycles,instructions,cpu_cycles,minor-faults,major-faults,page-faults,l1d_tlb,l2d_tlb,dtlb_walk,armv8_pmuv3/dTLB-loads/,armv8_pmuv3/dTLB-load-misses/,l1d_tlb_wr,l1d_tlb_refill,l2d_tlb_refill,l1i_tlb,itlb_walk,armv8_pmuv3/iTLB-loads/,armv8_pmuv3/iTLB-load-misses/,l1i_tlb_refill"

if [ $# -ne 2 ]; then
        echo "Usage: ${0} [workload] [output_file]"
        exit 1
fi

workload=${1}
out_file=${2}

out_path="/root/diploma_data"

echo "Preparing Workload Environment"
./enable_custom_hugepages.sh
./disable_custom_hugepages.sh
./enable_custom_hugepages.sh
echo "Environment Ready"

cd ~/spec
perf stat -e ${MEASUREMENTS} -o ${out_path}/${out_file}.stats --repeat 10 -- ./astar ./BigLakes2048.cfg
