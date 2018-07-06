#!/bin/bash

# compute the average of the average values for each of these timer events in a specified output file
# extra cruft to sort the results and discard the slowest one

for COUNTER in multiline_inline_rdtscp rdtsc inline_rdtsc_64 inline_rdtsc_32 rdtscp inline_rdtscp_64 inline_rdtscp_32 tacc_rdtscp rdpmc_instructions rdpmc_actual_cycles inline_rdpmc_actual_cycles_64 inline_rdpmc_actual_cycles_32 rdpmc inline_rdpmc_programmable_64 inline_rdpmc_programmable_32 rdpmc_reference_cycles inline_rdpmc_reference_cycles_64 inline_rdpmc_reference_cycles_32
do
	echo -n "${COUNTER} "
	grep "^${COUNTER} " $1 | awk 'START {max=0} {s+=$5; if($5>max) max=$5} END {print (s-max)/(NR-1)}'
done

