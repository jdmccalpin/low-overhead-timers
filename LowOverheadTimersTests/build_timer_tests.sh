#!/bin/bash

echo "Building for icc"
echo "  IMPORTANT NOTES:"
echo "    1. Choose a target architecture that does not include 256-bit or 512-bit SIMD support"
echo "       (Any SSE target will do -- the default for Linux is -msse2"
echo "    2. The compiler flag -nolib-inline prevents the compiler from generating calls to"
echo "       memset() or memcpy() that might contain 256-bit or 512-bit instructions."
echo " "
echo "Building for gcc"
echo "    1. Choose a target architecture that does not include 256-bit or 512-bit SIMD support"
echo "       (Any SSE target will do -- gcc defaults to -msse or -msse2.)"
echo "    2. The flag -fno-tree-loop-distribute-patterns will prevent the compiler from"
echo "       generating most calls to memset() or memcpy() (which might include 256-bit"
echo "       or 512-bit instructions."
echo " "

echo "compiling externally linked version with icc"
icc --version
icc -O2 -msse2 -nolib-inline -DUSE_PAUSE -c ../low_overhead_timers.c -o low_overhead_timers_icc.o
icc -O2 -msse2 -nolib-inline -DUSE_PAUSE -I.. test_timer_overhead.c low_overhead_timers_icc.o -qopt-report=5 -o timer_ovhd_external.icc.exe
mv test_timer_overhead.optrpt test_timer_overhead.external.optrpt
icc -O2 -msse2 -nolib-inline -DUSE_PAUSE -I.. test_timer_overhead.c -S -o timer_ovhd_external.icc.s

echo "compiling inlined version with icc"
icc -O2 -msse2 -nolib-inline -DUSE_PAUSE -DINLINE_TIMERS -I.. test_timer_overhead.c -qopt-report=5 -o timer_ovhd_inline.icc.exe
mv test_timer_overhead.optrpt test_timer_overhead.inline.optrpt
icc -O2 -msse2 -nolib-inline -DUSE_PAUSE -DINLINE_TIMERS -I.. test_timer_overhead.c -S -o timer_ovhd_inline.icc.s


echo "compiling externally linked version with gcc"
gcc --version
gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -c ../low_overhead_timers.c -o low_overhead_timers_gcc.o
gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -I.. test_timer_overhead.c low_overhead_timers_gcc.o -o timer_ovhd_external.gcc.exe
gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -I.. test_timer_overhead.c -fverbose-asm -S -o timer_ovhd_external.gcc.s

echo "compiling inlined version with gcc"
gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -DINLINE_TIMERS -I.. test_timer_overhead.c -o timer_ovhd_inline.gcc.exe
gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -DINLINE_TIMERS -I.. test_timer_overhead.c -fverbose-asm -S -o timer_ovhd_inline.gcc.s
