/*
*	Just a basic struct for loading a profile into the kernel.
*
*	For a first testing implementation the profile will hold an array
*	of memory ranges.
*
*	No actual decisions should be made with this profile structure as the
*	system will get heavily fragmented.
*
*	An array of 10 elements holding a cost and a benefit. Each address will
*	choose the cost and benefit based on address mod 10.
*
*	Just to check that we are able to access and load the profile through
*	eBPF maps.
*
*/
// Thelw diaforetiko cost kai benefit gia kathe megethos hp.
// To range start end tha einai to idio apla to cost/benefit tha einai diaforetika.
#define MAX_HP_ORDERS 9

enum hp_order {
	Page_16KB,
	Page_32KB,
	Page_64KB,
	Page_128KB,
	Page_256KB,
	Page_512KB,
	Page_1024KB,
	Page_2048KB
};

struct cb_struct {
	unsigned long long mybenefit;
	unsigned long long mycost;
};

struct prof_range {
	unsigned long long start;
	unsigned long long end;
	struct cb_struct cb[MAX_HP_ORDERS-1];
};

struct profile_t {
	int enabled;
	struct prof_range ranges[10];
};

