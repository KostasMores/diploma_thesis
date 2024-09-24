#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include "check-estimator.h"

/*
 * TODO: Need to refactor profile in order to take into account
 * mTHP cost / benefit.
 * 
 * For now load the profile into the 2 MB cost / benefit.
 * 
 * Write now the profile is formatted as:
 * benefit,cost
 * 
 * That should be changed to:
 * address.start,address.end,benefit_XB,cost_XB,...
 */
void print_profile(struct profile_t *p) {
	int i, k;
	printf("Printing Profile\n");
	printf("\tenabled: %d", p->enabled);
	printf("\tranges:\n");
	for (i = 0; i < 10; i++) {
		printf("\t\t[%llu, %llu]\n", p->ranges[i].start, p->ranges[i].end);
		for (k = 0; k < MAX_HP_ORDERS; k++) {
			printf("\t\t\torder %d, benefit: %llu, cost: %llu\n", k, 
									p->ranges[i].cb[k].mybenefit, 
									p->ranges[i].cb[k].mycost);
		}
		
	}
	return ;
}

struct profile_t * read_profile(char *filename)
{
	FILE *file;
	struct profile_t *profile;
	char line[1000];
	char *token;
	int i;
	int order;

	/* Allocate memory for the profile */
	profile = malloc(sizeof(struct profile_t));
	
	/* Open the profile */
	file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Error opening the file.\n");
        free(profile);
		return NULL;
	}
	
	/* Enable the profile */
	profile->enabled = 1;
	
	/* Read the profile */
	for (i = 0; i < 10; i++) {
		/* Read each line from the file */
		if(fgets(line, sizeof(line), file) == NULL) {
			printf("Error reading values from the file.\n");
			fclose(file);
            free(profile);
			return NULL;
		}
		/* Parse the line */
		printf("line: %s\n", line);
		/* Read start and end address of the range */
		token = strtok(line, ",");
		if (token == NULL) {
			printf("Error parsing the line (start).\n");
			fclose(file);
            free(profile);
			return NULL;
		}
		profile->ranges[i].start = strtoull(token, NULL, 0);
		token = strtok(NULL, ",");
		if (token == NULL) {
            printf("Error parsing the line (end).\n");
            fclose(file);
			free(profile);
            return NULL;
        }
		profile->ranges[i].end = strtoull(token, NULL, 0);
		
		/* Loop and read cost/benefit for each order */
		for (order = 0; order < MAX_HP_ORDERS; order++) {
			token = strtok(NULL, ",");
			if (token == NULL) {
				printf("Error parsing the line (benefit order %d).\n", order);
				fclose(file);
				free(profile);
				return NULL;
			}
			profile->ranges[i].cb[order].mybenefit = strtoull(token, NULL, 0);
			token = strtok(NULL, ",");
			if (token == NULL) {
				printf("Error parsing the line (cost order %d).\n", order);
				fclose(file);
				free(profile);
				return NULL;
			}
			profile->ranges[i].cb[order].mycost = strtoull(token, NULL, 0);
		}
	}

	fclose(file);
	return profile;
}

int main(int argc, char **argv)
{
	struct bpf_map_info info = {};
	unsigned int len = sizeof(info);
	int findme;
	int map_key;
	//long map_value = 2;
	struct profile_t *profile;

	if (argc != 3) {
		printf("Error correct usage: ./load_profile [pid] [profile]\n");
		return 1;
	} else {
		map_key = atoi(argv[1]);
		printf("Input read is: %d\n", map_key);
	}
	
	profile = read_profile(argv[2]);
	if (profile == NULL) {
		printf("Error reading profile from file.\n");
	}

	print_profile(profile);

	findme = bpf_obj_get("/sys/fs/bpf/my_map");

	if (findme <= 0) {
                printf("No FD\n");
	} else {
		bpf_obj_get_info_by_fd(findme, &info, &len);
		printf("name %s\n", info.name);
		printf("Trying to update the map from userspace...\n");
		if (bpf_map_update_elem(findme, &map_key, profile, BPF_ANY)) {
			fprintf(stderr, "Failed to update map element\n");
		}
		else {
			printf("Profile inserted.\n");
		}
	}

	free(profile);
	return 0;
}
