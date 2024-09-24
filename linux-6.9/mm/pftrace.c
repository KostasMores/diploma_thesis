#define CREATE_TRACE_POINTS
#include <trace/events/pftrace.h>

void do_trace_fault(unsigned long cycles) {
	trace_fault(cycles);
}

void do_trace_allochugepmdpage(unsigned long cycles) {
	trace_allochugepmdpage(cycles);
}

void do_trace_zerohugepmdpage(unsigned long cycles) {
	trace_zerohugepmdpage(cycles);
}

void do_trace_allocmthppage(unsigned long cycles) {
	trace_allocmthppage(cycles);
}

void do_trace_zeromthppage(unsigned long cycles) {
        trace_zeromthppage(cycles);
}

void do_trace_ebpfoverhead(unsigned long cycles) {
        trace_ebpfoverhead(cycles);
}

