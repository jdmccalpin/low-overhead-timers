This project contains a set of very low-overhead timers/counters for 
inline use in C code running on Intel64 processors.

These interfaces use the lowest-level hardware instructions to read
the Time Stamp Counter (TSC), the core fixed-function performance counters,
the core programmable performance counters, and a few auxiliary bits
of information.

These are intended for use in highly controlled tests.  The hardware
performance counters are private to each logical processor, so differences
only make sense between values that are guaranteed to have come from the
same logical processor.

These timers/counters employ user-space instructions only, and require
no elevated privileges.   Elevated privileges are required to program
to counters, but not to read them.  The sample scripts SetupPowerLevelCounters.sh
and SetupUserKernelCounters.sh require the msrtools binaries to be installed
and root permission to execute.  They set up the first four hardware performance
counters on each logical processor to measure events that are useful for
determining whether the output of the test_timer_overhead program is 
valid.

The interfaces provided include:

* rdtsc() returns the number of "nominal" processor cycles since the system booted in a 64-bit unsigned integer.
For all recent Intel processors, this counter increments at a fixed rate, independent of the actual
core clock speed or the energy-saving mode.
* rdtscp() is the same as rdtsc except that it is partially ordered -- it will not execute until all prior
instructions in program order have executed.  (See also full_rdtscp)
* full_rdtscp() returns the number of "nominal" processor cycles in a 64-bit unsigned integer and also 
modifies its two integer arguments to show the processor socket and processor core that were in use
when the call was made.  (Note: the various cores in a chip usually have very similar values for 
the TSC, but they are allowed to vary by processor.  This function guarantees that you know exactly
which processor the TSC reading came from.)
* get_core_number() uses the RDTSCP instruction, but returns only the core number in an integer variable.
* get_socket_number() uses the RDTSCP instruction, but returns only the socket number in an integer variable.
* rdpmc_instructions() uses a "fixed-function" performance counter to return the count of retired instructions on
the current core in the low-order 48 bits of an unsigned 64-bit integer.
* rdpmc_actual_cycles() uses a "fixed-function" performance counter to return the count of actual CPU core cycles
executed by the current core.  Core cycles are not accumulated while the processor is in the "HALT" state,
which is used when the operating system has no task(s) to run on a processor core.
* rdpmc_reference_cycles() uses a "fixed-function" performance counter to return the count of "reference" (or "nominal")
CPU core cycles executed by the current core.  This counts at the same rate as the TSC, but does not count
when the core is in the "HALT" state.  If a timed section of code shows a larger change in TSC than in
rdpmc_reference_cycles, the processor probably spent some time in a HALT state.
* rdpmc() reads the programmable core performance counter number specified in the input argument.
  No error or bounds checking is performed.

Auxiliary routines include:
* get_num_counters() uses the CPUID instruction to discover the number of programmable core counters
per logical processor.  This can be 2, 4, or 8, depending on model and mode of operation.
* get_core_counter_width() uses the CPUID instruction to discover the width of the programmable core 
performance counters.  This is 48 bits on most recent Intel processors, but processors with 40-bit and
32-bit counter widths also exist.
* corrected_pmc_delta() uses the counter width provided by get_core_counter_width() to compute a difference
between two counter values with corrections for a single wrap-around of the counter.
* get_TSC_frequency() parses the Brand Identification string from the CPUID instruction to get the "nominal"
frequency of the processor, which is also the invariant TSC frequency, and returned as a float value in Hz.
This can then be used to convert TSC cycles to seconds.


The source code "low_overhead_counters.c" can be used in one of two ways:
1. #include "low_overhead_counters.c" directly into your code, or
2. #include "low_overhead_counters.h" in your code and put
"low_overhead_counters.c" as a separate input on the compile
line, or compile it separately and include "low_overhead_counters.o"
on the compile/link line.

The second approach may add a few instructions and/or cycles to the 
overhead of the counters unless inter-procedural optimizations are applied.


A driver code and scripts for testing these interfaces are in the LowOverheadTimersTests subdirectory.
