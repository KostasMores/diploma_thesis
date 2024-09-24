#!/bin/bash

MEASUREMENTS="cycles,instructions,minor-faults,major-faults,dtlb_walk,armv8_pmuv3/dTLB-loads/,armv8_pmuv3/dTLB-load-misses/,itlb_walk,armv8_pmuv3/iTLB-loads/,armv8_pmuv3/iTLB-load-misses/"

if [ $# -ne 3 ]; then
        echo "Usage: ${0} [workload] [profile] [output_file]"
        exit 1
fi

workload=${1}
profile=${2}
out_file=${3}

out_path="/root/diploma_data"
script_path="/root/DiplomaThesis/ebpf/v9_load_profile"
pid=""

echo "Preparing Workload Environment"
./enable_custom_hugepages.sh
./disable_custom_hugepages.sh
./enable_custom_hugepages.sh
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

cd ${script_path}
./load_profile ${pid} ${profile}
cd ~
echo "Profile Loaded"

perf stat -e ${MEASUREMENTS} -o ${out_path}/${out_file}.stats --pid=${pid}
