#ifndef _MM_ECON_H_
#define _MM_ECON_H_

#include <linux/types.h>

#define MM_ACTION_NONE                 0
#define MM_ACTION_EBPF_HOOK	( 1 << 0 )
#define MM_ACTION_PMD		( 1 << 1 )
// An action that may be taken by the memory management subsystem.
struct mm_action {
    int action;

    u64 address;

    // Extra parameters of the action.
    unsigned long hugepage_orders;
};

// The cost of a particular action relative to the status quo.
struct mm_cost_delta {
    /* Total estimated cost in cycles */
    u64 cost;

    /* Total estimated benefit in cycles */
    u64 benefit;
};

bool mm_decide(const struct mm_cost_delta *cost);

void
mm_estimate_changes(const struct mm_action *action, struct mm_cost_delta *cost);

#endif
