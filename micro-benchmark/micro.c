#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

#define NUM_REGIONS 20000
#define REGION_SIZE (2 * 1024 * 1024)
#define FIRST_REGION_CNT 2500
#define INTERVAL (64 * 1024)
#define NUM_ACCESSES 128
#define TOTAL_ACCESSES 100000000
int access_value = 42;
int mthp_cnt = 0;
int thp_cnt = 0;

void access_random_page(void **regions, int num_regions, int count) {
	int acc_seed = rand();	// Replace acc_seed with call to rand()
	int region_index = acc_seed % num_regions;
	int access_mod = 1;

	if (region_index < FIRST_REGION_CNT) {
		/* Do a full access */
		access_mod = REGION_SIZE / sizeof(int);
		for (int i = 0; i < count; i++) {
			int random_offset = (acc_seed + i) % access_mod;
			((int *)regions[region_index])[random_offset] = access_value;
		}
		thp_cnt++;
	}
	else {
		/* Do 64KiB access */
		int middle_off = REGION_SIZE / 2;

		access_mod = INTERVAL / sizeof(int);
                for (int i = 0; i < count; i+=2) {
                        int random_offset = (acc_seed + i) % access_mod;
                        ((int *)regions[region_index])[random_offset] = access_value;
			((int *)regions[region_index])[random_offset + middle_off] = access_value;
		}
		mthp_cnt++;
	}

	return ;
}

int main() {
	srand(time(NULL));

	clock_t start, end;
	void *regions[NUM_REGIONS];
	FILE *file;
	
	sleep(5);
	/* Allocate the regions for the micro-benchmark */
	for (int i=0; i < NUM_REGIONS; i++) {
		regions[i] = mmap(NULL, REGION_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (regions[i] == MAP_FAILED) {
			perror("mmap failed");
			exit(EXIT_FAILURE);
		}
	}

	/* Implement the access logic 
	 * 	write a random value to fixed number of regions per time
	 * 	in count random number locations of this region
	 * 	This results in NUMBER_OF_RGNS * COUNT accesses per run.
	 */

	file = fopen("/tmp/alloctest-bench.ready", "w");
	start = clock();

	for (int i = 0; i < TOTAL_ACCESSES; i++) {
		access_value = rand();
		access_random_page(regions, NUM_REGIONS, NUM_ACCESSES);
	}

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
	printf("Accessed 100000 random pages 128 * 1024 times each.\n");
	printf("mTHP accesses: %d times, THP accesses: %d times.\n", mthp_cnt, thp_cnt);
	printf("Time taken: %f seconds\n", time_taken);
	return 0;
}
