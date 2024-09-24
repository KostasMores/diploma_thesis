#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage ./app_profile.sh [workload]"
	exit 1
fi

workload_name=${1}
files_path="./files/${workload_name}"

> ${workload_name}_avg.txt

for FILE in "${files_path}"/*; do
	if [ -f "$FILE" ]; then
		./profiler.py $FILE >> ${workload_name}_avg.txt
	fi
done

./avg_sort.py ${workload_name}_avg.txt ${workload_name}_avg_sorted.txt
