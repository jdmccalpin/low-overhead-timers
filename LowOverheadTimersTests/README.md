Test driver and scripts for the low_overhead_timers.c program.

The program "test_timer_overhead.c" tests all of the interfaces in
"low_overhead_counters.c" with 64 repeated calls, then reports the minimum,
average, and maximum counter deltas (excluding the first iteration).
The program also includes tests of the same counters read directly using
inline assembly macros for comparison.  The test_timer_overhead code is set
up so that it can be compiled in either inline or separate compilation mode.

The script "build_timer_tests.sh" will compile four versions of the
test_timer_overhead program -- one with inlining and one with separate
compilation, for each of the Intel (icc) and GNU (gcc) compilers.

The script "run_timer_test_ensemble.sh" will run each of these versions 10
times and save the results.

The script "summarize.sh" will take a result file from the
"run_timer_test_ensemble.sh" and compute the average of the average
values (after excluding the slowest result of the ensemble).

Building for icc
  IMPORTANT NOTES:
    1. Choose a target architecture that does not include 256-bit or 512-bit SIMD support
       (Any SSE target will do -- the default for Linux is -msse2)
    2. The compiler flag -nolib-inline prevents the compiler from generating calls to
       memset() or memcpy() that might contain 256-bit or 512-bit instructions.

Building for gcc
    1. Choose a target architecture that does not include 256-bit or 512-bit SIMD support
       (Any SSE target will do -- gcc defaults to -msse or -msse2.)
    2. The flag -fno-tree-loop-distribute-patterns will prevent the compiler from
       generating most calls to memset() or memcpy() (which might include 256-bit
       or 512-bit instructions).

Other compilers should be similar to gcc, but have not been carefully tested.
On Mac OS X, the clang compiler does not understand the gcc "-fno-tree-loop-distribute-patterns"
flag, but otherwise appears to work.

