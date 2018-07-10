#!/bin/bash

# You will need to put in the path to the msrtools binaries here
# if they are not in your default path.
# The msrtools commands almost always require root privileges.
WRMSR=wrmsr

# -------------------------------------------------------------------------
# This set of events is intended to help find instances in which the
# timer overhead test was contaminated by OS activity.
#
# PMC0 is set to measure actual cycles not halted in user or kernel mode.
#  This is an "architectural" event that should work on all Intel processors.
# PMC1 is set to measure actual cycles not halted in kernel mode.
#  This is an "architectural" event that should work on all Intel processors.
# PMC2 is set to measure instructions retired in kernel mode.
#  This is an "architectural" event that should work on all Intel processors.
# PMC3 is set to measure interrupts received (Skylake and later cores only)
#  PMC3 may report non-zero results on earlier Intel processors, but
#  those results will mean something unrelated to what I want to measure
#  here.
#
# A "clean" result has non-zero results in PMC0 and zero results in the
# other three counters.  
# For processors earlier than Skylake, ignore the results for PMC3.
# -------------------------------------------------------------------------

$WRMSR -a 0x186 0x0043003c		# actual cycles not halted (user + kernel)
$WRMSR -a 0x187 0x0042003c		# actual cycles not halted (kernel only)
$WRMSR -a 0x188 0x004200c0		# instructions retired (kernel only)
$WRMSR -a 0x189 0x004301cb		# interrupts received
