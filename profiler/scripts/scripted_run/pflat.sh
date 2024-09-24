#!/bin/bash
#
# Enabled / dump / disable the CA / ET/ PFTRACE events

set -e

EVENTS="/sys/kernel/debug/tracing/events"
HIST_TRIGGER="hist:keys=cycles.buckets=1:vals=hitcount:size=8192:sort=cycles"

enable() {
	pid=$1
	subsys=$2

	for event in ${EVENTS}/${subsys}/*; do
		if [[ -d "${event}" && -f "${event}/trigger" ]]; then
			echo "${HIST_TRIGGER} if common_pid == ${pid}" >> "${event}/trigger"
		fi
	done
}

pause() {
	subsys=$1

	for event in ${EVENTS}/${subsys}/*; do
		if [[ -d "${event}" && -f "${event}/trigger" ]]; then
			echo "${HIST_TRIGGER}:pause" >> "${event}/trigger"
		fi
	done
}

clr() {
	subsys=$1

	for event in ${EVENTS}/${subsys}/*; do
		if [[ -d "${event}" && -f "${event}/trigger" ]]; then
			echo "!${HIST_TRIGGER}" > "${event}/trigger"
		fi
	done
}

dump() {
	subsys=$1

	for event in ${EVENTS}/${subsys}/*; do
		if [[ -d "${event}" && -f "${event}/hist" ]]; then
			echo "${event}/hist"
			cat "${event}/hist"
		fi
	done
}

start_tracing() {
	FILTER_PIDS=$(pgrep -w ${1})

	for pid in $FILTER_PIDS; do
		enable $pid pftrace
	done
}

stop_tracing() {
	pause pftrace
}

clear_events() {
	clr pftrace
}

dump_hist() {
	dump pftrace
}

case "${1}" in
	"start")
		start_tracing ${2}
		;;
	"stop")
		stop_tracing
		;;
	"clear")
		clear_events
		;;
	"show")
		dump_hist
		;;
	*)
		start_tracing ${1}
		read -n1 -p "Press any key to continue..."
		dump_hist
		clear_events
esac
