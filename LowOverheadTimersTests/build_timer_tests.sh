#!/bin/bash

which icc >& /dev/null
if [ $? -ne 0 ]
then
    echo "Intel icc compiler not found, skipping...."
else
	echo "Intel icc compiler found...."
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
fi

which gcc >& /dev/null
if [ $? -ne 0 ]
then
    echo "GNU gcc compiler not found, skipping...."
else
	echo "GNU gcc compiler found...."
	echo "compiling externally linked version with gcc"
	gcc --version
	gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -c ../low_overhead_timers.c -o low_overhead_timers_gcc.o
	gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -I.. test_timer_overhead.c low_overhead_timers_gcc.o -o timer_ovhd_external.gcc.exe
	gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -I.. test_timer_overhead.c -fverbose-asm -S -o timer_ovhd_external.gcc.s

	echo "compiling inlined version with gcc"
	gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -DINLINE_TIMERS -I.. test_timer_overhead.c -o timer_ovhd_inline.gcc.exe
	gcc -O2 -msse2 -fno-tree-loop-distribute-patterns -DUSE_PAUSE -DINLINE_TIMERS -I.. test_timer_overhead.c -fverbose-asm -S -o timer_ovhd_inline.gcc.s
fi
