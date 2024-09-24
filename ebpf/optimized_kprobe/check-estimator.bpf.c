#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "check-estimator.h"

/*
 * The Linux Kernel provides a total of 8 different 
 * huge page sizes. These are the following:
 * 
 * 16	kB	->	order 2
 * 32	kB	->	order 3
 * 64	kB	->	order 4
 * 128	kB	->	order 5
 * 256	kB	->	order 6
 * 512	kB	->	order 7
 * 1024	kB	->	order 8
 * 2048	kB	->	order 9		(pmd sized)
 *
 * The order is calculated the following way:
 * The size of the allocated huge page is calculated as:
 * 
 * 	HUGE_PAGE_SIZE	=	PAGE_SIZE	<<	order
 */

/*	TODO: kfunc implementation
extern void bpf_update_profile(struct mm_action *action,
								struct mm_cost_delta *cost,
								int bpf_action, u64 bpf_cost, u64 bpf_benefit) __ksym;
*/

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
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

/*
 * This function will take the address as an argument and return the index 
 * where the given cost and benefit for the given address is stored.
 */

/*
int get_profile_from_range(struct profile_t *profile, unsigned long address) {
	int res = -1;
	int i;
	// Check if we need to use BPF_PROBE_READ 
	for (i = 0; i < 10; i++) {
		if (profile->ranges[i].start < address && profile->ranges[i].end > address) {
			res = i;
			bpf_printk("Found profile range for faulting address: %d\n", res);
			break;
		}
	}

	return res;
}
*/

SEC("kprobe/mm_estimate_changes")
int BPF_KPROBE(mm_estimate_changes, struct mm_action *action,
					 struct mm_cost_delta *cost)
{
	struct profile_t data = {};
	struct profile_t *profile = NULL;

	unsigned long temp_bit;

	/* Order from the kernel */
	unsigned long available_orders;
	int high_order;

	/* Faulting address */
	u64 fault_address;

	/* Map access fields */
	pid_t pid = bpf_get_current_pid_tgid() >> 32;
	int index = (int) pid;
	int my_action = (1 << 0);
	int kernel_action;

	/* eBPF calculated order */
	unsigned long beneficial_orders = 0;
	int best_order;
	unsigned long final_orders;

	/* Read the kernel action */
	kernel_action = BPF_CORE_READ(action, action);
	//kernel_action = action->action;
	// bpf_printk("kernel action: %d\n", kernel_action);

	/* Read the kernel huge page orders from the kernel */
	available_orders = BPF_CORE_READ(action, hugepage_orders);
	//available_orders = action->hugepage_orders;
	high_order = highest_order(available_orders);
	// bpf_printk("mm_estimate_changes, ENTRY pid = %d\n", pid);
	// bpf_printk("\t\tAvailable orders for this page fault: %lu, High order: %d", available_orders, high_order);

	/* Read the faulting address from the kernel */
	fault_address = BPF_CORE_READ(action, address);
	//fault_address = action->address;
	// bpf_printk("\t\tPage fault address: %llu\n", fault_address);

	/* Sanity check for not out of bounds checking in eBPF map array */
	if (high_order > (MAX_HP_ORDERS)) {
		// bpf_printk("[ERROR]: Bad order from kernel\n");
		return 0;
	}
	
	// Access the map "my_map"
	profile = bpf_map_lookup_elem(&my_map, &index);
	if (profile) {
		// data.enabled = BPF_PROBE_READ(profile, enabled);
		if (profile->enabled == 1) {
			int ij;
			int order = 0;
			unsigned long long profit = 0;
			unsigned long long temp;
			int order_index = -1;
			int range_index = -1;

			//bpf_printk("\t\t Found corresponding value for map entry.\n");
			/* Get the the correct profile for the faulting address */
			// range_index = get_profile_from_range(profile, fault_address);

			for (ij = 0; ij < 10; ij++) {
				if (profile->ranges[ij].start < fault_address && profile->ranges[ij].end > fault_address) {
					range_index = ij;
					bpf_printk("Found profile range for faulting address: %d\n", range_index);
					break;
				}
			}

			if (range_index < 0 || range_index > 10) {
				bpf_printk("\t\tDidn't find profile range for the faulting address\n");
				return 0;
			}

			/* [DEBUG]: Print the benefit and cost for the huge page order 2 for sanity check */
			// data.ranges[range_index].cb[order].mybenefit = BPF_PROBE_READ(profile, ranges[range_index].cb[order].mybenefit);
			// data.ranges[range_index].cb[order].mycost = BPF_PROBE_READ(profile, ranges[range_index].cb[order].mycost);
			// bpf_printk("\t\t The profile is: %llu, %llu\n", data.ranges[range_index].cb[order].mybenefit,
			//			data.ranges[range_index].cb[order].mycost);
			
			/* [DEBUG]: Print the address range of the profile we picked */
			// bpf_printk("\t\t Starting search for best huge page size...\n");
			// bpf_printk("\t\t range: [%llu, %llu]\n", profile->ranges[range_index].start, profile->ranges[range_index].end);

			for (order = high_order - 1; order > -1; order--) {
				if (profile->ranges[range_index].cb[order].mybenefit > profile->ranges[range_index].cb[order].mycost)
				{
					/* First set the bit in beneficial orders */
					beneficial_orders |= (1 << (order+1));
					temp = profile->ranges[range_index].cb[order].mybenefit - profile->ranges[range_index].cb[order].mycost;
					if (temp > profit) {
						profit = temp;
						order_index = order;
						beneficial_orders = (1 << (order+1));
					}
				}
			}
			/* order_index has values [0,7] since it is in an array */
			if (order_index != -1) {
				// bpf_printk("\t\t Most beneficial huge page of order: %d, profit: %llu\n", order_index+1, profit);
				/* 
				 * Then keep only the orders that are:
				 * - beneficial from the profile and
				 * - smaller than the most beneficial one (this one is implemented in the for loop search).
				 */
				final_orders = beneficial_orders & available_orders;
			}
			else {
				bpf_printk("\t\t No benefit from using huge pages.\n");
				final_orders = 0;
			}
			//bpf_printk("Orders sending to the kernel: %llu\n", final_orders);
			high_order = highest_order(final_orders);

			if (final_orders != 0) {
				bpf_printk("Huge page should be promoted to order or less: %d\n", high_order);
			}

			bpf_update_action(action, my_action);
			bpf_update_orders(action, final_orders);
			//bpf_printk("\t\t Injected values.\n");
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
