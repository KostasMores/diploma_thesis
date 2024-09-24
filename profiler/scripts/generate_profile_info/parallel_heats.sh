#!/bin/bash

if [ "$#" -ne 1 ]; then
        echo "Usage: ${0} [workload_name]"
        exit 1
fi

workload=${1}

addr_range_start=$((0xfffff7a40000))
addr_range_end=$((0xfffff8000000))
addr_range_size=$((addr_range_end - addr_range_start))

addr_range_size_KB=$((addr_range_size / 1024))
addr_range_size_MB=$((addr_range_size_KB / 1024))

echo "Address Range Size: ${addr_range_size} bytes -> ${addr_range_size_MB} MiB"

image_counter=0
addr_range_loop=$((addr_range_start))
addr_range_next=$((addr_range_loop))
inc_2MB=$((0x200000))

parallel_idx=10
inner_loop=0

while [ $addr_range_loop -lt $addr_range_end ]; do
	let "addr_range_next = addr_range_loop + inc_2MB"
	let "inner_loop = 0"

	while [ $inner_loop -lt $parallel_idx ] && [ $addr_range_loop -lt $addr_range_end ]; do
		((image_counter++))
		damo report heats -i ${workload}_native.data --resol 1000 1 --address_range ${addr_range_loop} ${addr_range_next} --abs_addr > ${workload}_${addr_range_loop}.txt &
		((inner_loop++))
		let "addr_range_loop += inc_2MB"
	done

	wait
done

echo "Expecting ${image_counter} images"
