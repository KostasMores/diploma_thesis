#!/bin/bash

benchmark_path="/root/XSBench/openmp-threading"
script_path="/root/profiler/scripts/scripted_run"

for i in $(seq 1 5); do
	./disable_custom_hugepages.sh
	./setup_env.sh
	./noalloc_run_xsbench.sh XSBench "noalloc_nohugevanilla${i}" &
	pid=$!
	cd ${benchmark_path}
	./XSBench -t 1 -s XL -l 250
	cd ${script_path}
	wait "$pid"
	echo "End of iteration ${i}"
	sleep 60
done

