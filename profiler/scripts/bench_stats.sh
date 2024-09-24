#!/bin/bash

MEASUREMENTS="cycles,instructions,minor-faults,major-faults,dtlb_walk,armv8_pmuv3/dTLB-loads/,armv8_pmuv3/dTLB-load-misses/,itlb_walk,armv8_pmuv3/iTLB-loads/,armv8_pmuv3/iTLB-load-misses/"

if [ $# -ne 1 ]; then
        echo "Usage ./script [workload]"
        exit 1
fi

workload=${1}

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

perf stat -e ${MEASUREMENTS} -o canneal_native_huge.stats --pid=${pid}
