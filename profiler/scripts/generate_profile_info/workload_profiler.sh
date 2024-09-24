#!/bin/bash


if [ "$#" -ne 4 ]; then
	echo "Usage: ${0} [workload_name] [output_file] [min_regions] [max_regions]"
	exit 1
fi

workload=${1}
out_file=${2}
min_regions=${3}
max_regions=${4}
pid=""

if [ "${max_regions}" -lt "${min_regions}" ]; then
	echo "[ERROR]: max_regions CAN NOT be less than min_regions"
	exit 1
fi

echo "Preparing Workload Environment"
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

damo record --ops vaddr --target_pid ${pid} --minr ${min_regions} --maxr ${max_regions} --out ./${out_file}.data --include_child_tasks
