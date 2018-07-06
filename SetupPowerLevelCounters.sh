#!/bin/bash

# You will need to put in the path to the msrtools binaries here
# if they are not in your default path.
# The msrtools commands almost always require root privileges.
WRMSR=wrmsr

# This set of events is specific to the Intel Xeon Scalable Processor
# family (Skylake Xeon).
# The desired result for the test_timer_overhead program is that
# "actual cycles not halted" matches "core power level 0 cycles", 
# and that the "core power level 1 cycles" and "core power 
# level 2 cycles" counters are zero.
# If the latter two are *not* zero, the power control unit in
# the processor will halt the core while it adjusts the voltage
# and activates the 256-bit or 512-bit pipelines.  The timing
# of this halt is unpredictable, and the duration of the halt
# can be 20,000 or more core cycles.


$WRMSR -a 0x186 0x0043003c		# actual cycles not halted
$WRMSR -a 0x187 0x00430728		# core power level 0 cycles
$WRMSR -a 0x188 0x00431828		# core power level 1 cycles
$WRMSR -a 0x189 0x00432028		# core power level 2 cycles
