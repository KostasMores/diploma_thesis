#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <signal.h>

#define NUM_REGIONS 46
#define REGION_2MiB (2 * 1024 * 1024)
#define REGION_64KiB (64 * 1024)
#define REGION_4KiB (4 * 1024)
#define DTLB_L1_SIZE 46
#define TLB_L2_SIZE 1280	// The L2 TLB Cache is shared between data and instructions. Also it is 5-way set associative.
#define REGION_SIZE (2 * 1024 * 1024)
#define FIRST_REGION_CNT 10
#define ITERATIONS 1
#define INNER_ITRS 10000000

int mthp_cnt = 0;
int thp_cnt = 0;

/* Set random values on each page of the memory areas */
void prepare_benchmark(void **regions) {
	for (int i = 0; i < NUM_REGIONS; i++) {
		int init_val = rand();
		//printf("Preparing Region: %d\n", i);
		for (int j = 0; j < REGION_2MiB; j += REGION_4KiB) {
			((int *)regions[i])[j / sizeof(int)] = init_val;
		}
	}
	return ;
}

int * get_random_regions() {
	int *indexes = (int*)malloc(DTLB_L1_SIZE * sizeof(int));

	for (int i = 0; i < DTLB_L1_SIZE; i++) {
		//int seed = rand();
		indexes[i] = i;
	}

	return indexes;
}

void free_random_regions(int * random_regions) {
	free(random_regions);
	random_regions = NULL;
	
	return;
}

void calculate(void **regions, int *indx, int calc_seed) {
	int thp_value = 0;
	int mthp_value = 0;
	int final_value = 0;

	for (int w = 0; w < INNER_ITRS; w++) {
		for (int i = 0; i < DTLB_L1_SIZE; i++) {
                	if (indx[i] < FIRST_REGION_CNT) {
                        	for (int j = 0; j < REGION_2MiB; j += REGION_4KiB) {
                                	thp_value += (((int *)regions[indx[i]])[j / sizeof(int)]);
                                	thp_value = thp_value % calc_seed;
                        	}
                        	thp_cnt++;
                	}
                	else {
                        	for (int j = 0; j < REGION_64KiB; j += REGION_4KiB) {
                                	mthp_value += (((int *)regions[indx[i]])[j / sizeof(int)]);
                                	mthp_value = mthp_value % calc_seed;
                        	}
                        	mthp_cnt++;
                	}

                	final_value = thp_value % mthp_value;

                	if (indx[i] < FIRST_REGION_CNT) {
                        	for (int j = 0; j < REGION_2MiB; j += REGION_4KiB) {
                                	((int *)regions[indx[i]])[j / sizeof(int)] = final_value;
                        	}
                	}
                	else {
                        	for (int j = 0; j < REGION_64KiB; j += REGION_4KiB) {
                                	((int *)regions[indx[i]])[j / sizeof(int)] = final_value;
                        	}
                	}
        	}
	}

	return;
}

void run_benchmark(void **regions) {
	int *indx;
	int calc_seed = rand();
	indx = get_random_regions();
	
	calculate(regions, indx, calc_seed);
	
	free_random_regions(indx);
	return ;
}

void benchmark(void **regions, int iterations) {
	for (int i = 0; i < iterations; i++) {
		run_benchmark(regions);
	}
}

void print_regions(void **regions) {
	printf("First 2MiB region address: %p\n", regions[0]);
	printf("Last  2MiB region address: %p\n", regions[FIRST_REGION_CNT-1]);
	printf("First 64KiB region address: %p\n", regions[FIRST_REGION_CNT]);
	printf("Last  64KiB region address: %p\n", regions[NUM_REGIONS-1]);
}

void handle_signal(int signal) {
	printf("Continuing execution...\n");
}

int main() {
	srand(time(NULL));

	clock_t start, end;
	void *regions[NUM_REGIONS];
	FILE *file;
	
	signal(SIGUSR1, handle_signal);
	printf("Waiting for signal\n");
	pause();

	/* Allocate the regions for the micro-benchmark */
	for (int i=0; i < NUM_REGIONS; i++) {
		regions[i] = mmap(NULL, REGION_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (regions[i] == MAP_FAILED) {
			perror("mmap failed");
			exit(EXIT_FAILURE);
		}
	}

	printf("Preparing...\n");
	prepare_benchmark(regions);

	print_regions(regions);

	file = fopen("/tmp/alloctest-bench.ready", "w");
	start = clock();

	printf("Running...\n");
	benchmark(regions, ITERATIONS);
	printf("Done.\n");

	end = clock();

	/* Free the allocated regions of the micro-benchmark */
	for (int i = 0; i < NUM_REGIONS; i++) {
		if (munmap(regions[i], REGION_SIZE) == -1) {
			perror("munmap failed");
		}
	}

	fclose(file);
	double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

	printf("Benchmark completed successfully.\n");
	printf("mTHP accesses: %d times, THP accesses: %d times.\n", mthp_cnt / INNER_ITRS, thp_cnt / INNER_ITRS);
	printf("Time taken: %f seconds\n", time_taken);
	return 0;
}
