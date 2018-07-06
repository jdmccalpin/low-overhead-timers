#!/bin/bash

# You will need to put in the path to the msrtools binaries here
# if they are not in your default path.
# The msrtools commands almost always require root privileges.
WRMSR=wrmsr

# This set of events is intended to help find instances in which the
# timer overhead test was contaminated by OS activity.

$WRMSR -a 0x186 0x0043003c		# actual cycles not halted (user + kernel)
$WRMSR -a 0x187 0x0042003c		# actual cycles not halted (kernel only)
$WRMSR -a 0x188 0x004200c0		# instructions retired (kernel only)
$WRMSR -a 0x189 0x004301cb		# interrupts received
