#!/bin/bash

MEASUREMENTS="cycles,instructions,cpu_cycles,minor-faults,major-faults,page-faults,l1d_tlb,l2d_tlb,dtlb_walk,armv8_pmuv3/dTLB-loads/,armv8_pmuv3/dTLB-load-misses/,l1i_tlb,itlb_walk,armv8_pmuv3/iTLB-loads/,armv8_pmuv3/iTLB-load-misses/"

if [ $# -ne 2 ]; then
        echo "Usage: ${0} [workload] [output_file]"
        exit 1
fi

workload=${1}
out_file=${2}

out_path="/root/final_data"
pid=""

echo "Preparing Workload Environment"
./setup_env.sh
./enable_custom_hugepages.sh
./disable_custom_hugepages.sh
echo "Environment Ready"

echo "Waiting to start the Workload"
while true; do
	pid=$(pidof ${workload})
	if [ -n "$pid" ]; then
		echo "$pid"
		break
	fi
	sleep 0.1
done
echo "Workload started"

./pflat.sh "start" ${workload}
perf stat -e ${MEASUREMENTS} -o ${out_path}/${out_file}.stats --pid=${pid}
./pflat.sh "stop"
./pflat.sh "show" > "${out_path}/${out_file}.hist"
./pflat.sh "clear"

