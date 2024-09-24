#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include "check-estimator.h"

int main(int argc, char **argv)
{
	struct bpf_map_info info = {};
	unsigned int len = sizeof(info);
	int findme;
	int map_key;
	struct profile_t profile;

	if (argc != 2) {
		printf("Error correct usage: ./load_profile [pid]\n");
		return 1;
	} else {
		map_key = atoi(argv[1]);
		printf("PID read is: %d\n", map_key);
	}

    memset(&profile, 0, sizeof(profile));

	findme = bpf_obj_get("/sys/fs/bpf/my_map");

	if (findme <= 0) {
                printf("No FD\n");
	} else {
		bpf_obj_get_info_by_fd(findme, &info, &len);
		printf("name %s\n", info.name);
		printf("Trying to update the map from userspace...");
		if (bpf_map_update_elem(findme, &map_key, &profile, BPF_ANY)) {
			fprintf(stderr, "Failed to update map element\n");
		}
		else {
			printf("Profile deleted.\n");
		}
	}

	return 0;
}