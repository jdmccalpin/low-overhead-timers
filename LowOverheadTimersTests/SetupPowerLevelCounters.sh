#!/bin/bash

# You will need to put in the path to the msrtools binaries here
# if they are not in your default path.
# The msrtools commands almost always require root privileges.
WRMSR=wrmsr

# -------------------------------------------------------------------
# PMC0 on each core is set to record actual cycles not halted (in
# either user or kernel mode).  This event is "architectural", so 
# it should work on almost all Intel processors.
# -------------------------------------------------------------------
# The next three events are specific to the Intel Xeon Scalable
# Processor family (Skylake Xeon).
#  --> These event encodings might produce non-zero results on
#      other Intel processors (Event 0x28 was used for L1D cache
#      writebacks to L2 on Nehalem/Westmere/SandyBridge/IvyBridge),
#      but these specific values are intended for SKX only.
# -------------------------------------------------------------------
# The desired result for the test_timer_overhead program using these
# Skylake Xeon counters is that PMC0 ("actual cycles not halted")
# matches PMC1 ("core power level 0 cycles"), and that PMC2 
# ("core power level 1 cycles") and PMC3 ("core power level 2 cycles")
# counters are zero.
# If PMC2 and PMC3 are *not* zero, the power control unit in
# the processor will halt the core while it adjusts the voltage
# and activates the 256-bit or 512-bit pipelines.  The timing
# of this halt is unpredictable, and the duration of the halt
# can be 20,000 or more core cycles.
# -------------------------------------------------------------------

$WRMSR -a 0x186 0x0043003c		# actual cycles not halted
$WRMSR -a 0x187 0x00430728		# core power level 0 cycles
$WRMSR -a 0x188 0x00431828		# core power level 1 cycles
$WRMSR -a 0x189 0x00432028		# core power level 2 cycles
