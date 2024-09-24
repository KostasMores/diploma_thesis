#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "check-estimator.h"

/*
 * The Linux Kernel provides a total of 8 different 
 * huge page sizes. These are the following:
 * 
 * 16	kB	->	order 1
 * 32	kB	->	order 2
 * 64	kB	->	order 3
 * 128	kB	->	order 4
 * 256	kB	->	order 5
 * 512	kB	->	order 6
 * 1024	kB	->	order 7
 * 2048	kB	->	order 8		(pmd sized)
 */

/*	TODO: kfunc implementation
extern void bpf_update_profile(struct mm_action *action,
								struct mm_cost_delta *cost,
								int bpf_action, u64 bpf_cost, u64 bpf_benefit) __ksym;
*/
struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__type(key, u32);
	__type(value, struct profile_t);
	__uint(max_entries, 10240);
	__uint(pinning, LIBBPF_PIN_BY_NAME);
} my_map SEC(".maps");

int highest_order(unsigned long a) {
	int r = 0;
	
	if (!a)
		return -1;
	
	while (a >>= 1)
		r++;
	
	return r;
}

SEC("kprobe/mm_estimate_changes")
int BPF_KPROBE(mm_estimate_changes, struct mm_action *action,
					 struct mm_cost_delta *cost)
{
	struct profile_t data = {};
	struct profile_t *profile = NULL;
/*
 *	Commented out code is for testing argument accessing in eBPF.
	struct mm_cost_delta readcost = {};
	// mm_action fields
	int paction;
	u64 address;
	// mm_cost_delta fields
	u64 pcost, benefit;
*/
	unsigned long available_orders;
	int high_order;
	// map access fields.
	pid_t pid = bpf_get_current_pid_tgid() >> 32;
	int index = (int) pid;
	int my_action = (1 << 4);

/*
	// Access kernel data structures.
	paction = BPF_CORE_READ(action, action);
	address = BPF_CORE_READ(action, address);
	pcost = BPF_CORE_READ(cost, cost);
	benefit = BPF_CORE_READ(cost, benefit);

	readcost.cost = pcost;
	readcost.benefit = benefit;

	bpf_printk("mm_estimate_changes, ENTRY pid = %d\n", pid);
	bpf_printk("\t\t action : %d | address : %llu\n", paction, address);
	bpf_printk("\t\t cost : %llu | benefit : %llu\n", pcost, benefit);
	bpf_printk("\t\t cost : %llu | benefit : %llu\n", readcost.cost, readcost.benefit);
*/

	available_orders = BPF_CORE_READ(action, hugepage_orders);
	high_order = highest_order(available_orders);
	bpf_printk("mm_estimate_changes, ENTRY pid = %d\n", pid);
	bpf_printk("/t/tAvailable orders for this page fault: %lu, High order: %d", available_orders, high_order);
	// Access the map "my_map"
	profile = bpf_map_lookup_elem(&my_map, &index);
	if (profile) {
		data.enabled = BPF_PROBE_READ(profile, enabled);
		if (data.enabled == 1) {
			int order = 0;
			unsigned long long profit = 0;
			unsigned long long temp;
			int order_index = -1;

			data.ranges[0].cb[order].mybenefit = BPF_PROBE_READ(profile, ranges[0].cb[order].mybenefit);
			data.ranges[0].cb[order].mycost = BPF_PROBE_READ(profile, ranges[0].cb[order].mycost);
			bpf_printk("\t\t Found corresponding value for map entry.\n");
			bpf_printk("\t\t The profile is: %llu, %llu\n", data.ranges[0].cb[order].mybenefit,
						data.ranges[0].cb[order].mycost);
			// Push the values of  the profile to the kernel
			// TODO: Do this with kfuncs instead of helpers.
			bpf_printk("\t\t Starting search for best huge page size...\n");
			bpf_printk("\t\t range: [%llu, %llu]\n", profile->ranges[0].start, profile->ranges[0].end);
			for (order = 0; order < MAX_HP_ORDERS; order++) {
				if (profile->ranges[0].cb[order].mybenefit > profile->ranges[0].cb[order].mycost)
				{
					temp = profile->ranges[0].cb[order].mybenefit - profile->ranges[0].cb[order].mycost;
					if (temp > profit) {
						profit = temp;
						order_index = order;
					}
				}
				bpf_printk("\t\t\t profit: %llu\n", profit);
			}
			if (order_index != -1) {
				bpf_printk("\t\t Most beneficial huge page of order: %d, profit: %llu\n", order_index, profit);
			}
			else {
				bpf_printk("\t\t No benefit from using huge pages.\n");
			}
			
			bpf_update_action(action, my_action);
			bpf_update_profile(cost, data.ranges[0].cb[order].mycost, data.ranges[0].cb[order].mybenefit);
			bpf_printk("\t\t Injected values.\n");
		}
		else {
			bpf_printk("\t\t No active loaded profile found for this process.\n");
		}
		/* Need to either a) write a bpf helper to update or
		b) write a func and expose as a kfunc */
		
		/* TODO: kfunc attempt.
		bpf_update_profile(action, cost, (1 << 7), data.ranges[0].mycost,
												 	data.ranges[0].mybenefit);
		*/
	}
	else {
		bpf_printk("\t\t No map entry was found for this process.\n");
	}

	return 0;
}

char LICENCE[] SEC("license") = "Dual BSD/GPL";
