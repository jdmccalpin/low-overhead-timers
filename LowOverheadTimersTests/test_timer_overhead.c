#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

// ----------IMPORTANT -----------
//    Use the INLINE_TIMERS preprocessor variable to determine
//    whether the source code to the timers is included here
//    or just the headers (for separate compilation and linking).
//
#ifdef INLINE_TIMERS
#include "low_overhead_timers.c"
#else
#include "low_overhead_timers.h"
#endif

#define inline_rdpmc(hi,low,counter) \
   __asm__ volatile("rdpmc" : "=a" (low), "=d" (hi) : "c" (counter));

#define inline_rdtsc(hi,low) \
	__asm__ volatile("rdtsc": "=a" (low), "=d" (hi));

#define inline_rdtscp(hi,low,aux) \
	__asm__ volatile("rdtscp": "=a" (low), "=d" (hi), "=c" (aux));

# define NUM_CORE_COUNTERS 2







#define FATAL(fmt,args...) do {                \
    ERROR(fmt, ##args);                        \
    exit(1);                                   \
  } while (0)

#define ERROR(fmt,args...) \
    fprintf(stderr, fmt, ##args)

#define NSAMPLES 64
#define NHALFSAMPLES 256

unsigned long values64[NSAMPLES];
unsigned int values32[NSAMPLES];
unsigned int highlow32[NHALFSAMPLES];


#define INNERTIMES (10000)
#define MIDDLETIMES (1000)

// This function can be extracted for standalone use....
// If USE_PAUSE is not defined, it will spin very quickly
// (>1 billion increments per second).
// If USE_PAUSE is defined, it will spin very slowly --
// between ~4 cycles and ~26 cycles per PAUSE instruction depending
// on the processor model.
unsigned long spin_function(unsigned long initial_counter)
{
	int middle, inner;
#pragma nounroll
		for (middle=0; middle<MIDDLETIMES; middle++) {
#pragma nounroll
			for (inner=0; inner<INNERTIMES; inner++) {
#ifdef USE_PAUSE
				__asm__ volatile("pause");
				__asm__ volatile("pause");
				__asm__ volatile("pause");
				__asm__ volatile("pause");
#endif
				initial_counter++;
			}
		}
	return(initial_counter);
}


int main ( int argc, char *argv[] )
{
	int cpu;
	int chip, core;
	int i,j,k,l,counter;
	unsigned int low, high, aux;
	unsigned long count;
	unsigned long dummy;
	int core_ctr_width;
	int fixed_ctr_width;

	unsigned long gen_ins_start, gen_ins_end, fix_ins_start, fix_ins_end;
	unsigned long gen_cyc_start, gen_cyc_end, fix_cyc_start, fix_cyc_end;
	unsigned long gen_ref_start, gen_ref_end, fix_ref_start, fix_ref_end;
	unsigned long tsc_start, tsc_end,  tscp_start, tscp_end;
	unsigned long core_counter_start[NUM_CORE_COUNTERS],core_counter_end[NUM_CORE_COUNTERS];
	long delta_cycle, delta_tsc;
	long minval,maxval;
	double avgval;
	double spintime;
	register unsigned long tmp64;
	char FUNCTIONLABEL[100];

	float utilization, nominal_ghz, avg_ghz;

	nominal_ghz = get_TSC_frequency() / 1.0e9;
	printf("Nominal GHz %f\n",nominal_ghz);

	core_ctr_width = get_core_counter_width();
	fixed_ctr_width = get_fixed_counter_width();
	printf("programmable core counter width is %d bits\n",core_ctr_width);
	printf("fixed-function core counter width is %d bits\n",fixed_ctr_width);
	if (core_ctr_width != fixed_ctr_width) {
		printf("Warning -- programmable counter width does not match fixed-function counter width -- this code may have errors!\n");
	}

	// bind to a single processor
	cpu = 4;			
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(cpu, &cpu_set);
    if (sched_setaffinity(0, sizeof(cpu_set), &cpu_set) < 0) FATAL("cannot set cpu affinity: %m\n");
	printf("Affinity set to cpu %d\n",cpu);
	printf("  get_core_number returns %d\n",get_core_number());
	printf("  get_socket_number returns %d\n",get_socket_number());
	count = full_rdtscp(&chip, &core);
	printf("  full_rdtsc returns chip %d, core %d\n",chip,core);

	printf("Spinning for a short time to allow the processor to ramp up to full speed\n");
	dummy = 0;
	for (i=0; i<NUM_CORE_COUNTERS; i++) core_counter_start[i] = rdpmc(i);
	fix_ins_start = rdpmc_instructions();
	tsc_start = rdtscp();
	count = spin_function(dummy);
	tsc_end = rdtscp();
	fix_ins_end = rdpmc_instructions();
	for (i=0; i<NUM_CORE_COUNTERS; i++) core_counter_end[i] = rdpmc(i);
	spintime = (double)(tsc_end - tsc_start) / nominal_ghz / 1.0e9;
	printf("  Spinning for %lu increments took %f seconds and executed %lu instructions\n",count,spintime,fix_ins_end - fix_ins_start);
	printf("COUNTERS: spinning core count deltas ");
	for (i=0; i<NUM_CORE_COUNTERS; i++) {
		printf(" %lu",corrected_pmc_delta(core_counter_end[i],core_counter_start[i],core_ctr_width));
	}
	printf("\n");
	// the get_core_counter_width() function uses the cpuid instruction, which will serialize
	// execution and try to keep any rdtsc instructions in the next test from getting too far out of order
	count += get_core_counter_width();

// --- 0 ---
	strcpy(FUNCTIONLABEL,"multiline_inline_rdtscp");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) {
		__asm__ volatile(
		 "\
		   rdtscp                           \n\
		   shl      $32, %%rdx              \n\
		   orq      %%rax, %%rdx            \n\
		   movq     %%rdx, %[cyc]"
		   :
		   /*outputs here*/
		   [cyc]    "=r" (tmp64)
		   :
		   /*inputs here*/
		   :
		   /*clobbered registers*/
		   "rax","eax","rcx","ecx","rdx"
		);
		values64[j] = tmp64;
	  }
#include "counter_test_epilog.c"

// --- 1 ---
	strcpy(FUNCTIONLABEL,"rdtsc");
#include "counter_test_prolog.c"
for (j=0; j<NSAMPLES; j++) values64[j] = rdtsc();
#include "counter_test_epilog.c"

// --- 2 ---
	strcpy(FUNCTIONLABEL,"inline_rdtsc_64");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdtsc(high,low);
        values64[j] = ((unsigned long)high<<32) | (unsigned long)low;
      }
#include "counter_test_epilog.c"

// --- 3 ---
	strcpy(FUNCTIONLABEL,"inline_rdtsc_32");
#include "counter_test_prolog_32.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdtsc(high,low);
        values32[j] = low;
      }
#include "counter_test_epilog_32.c"

// --- 4 ---
	strcpy(FUNCTIONLABEL,"rdtscp");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) values64[j] = rdtscp();
#include "counter_test_epilog.c"

// --- 5 ---
	strcpy(FUNCTIONLABEL,"inline_rdtscp_64");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdtscp(high,low,aux);
        values64[j] = ((unsigned long)high<<32) | (unsigned long)low;
      }
#include "counter_test_epilog.c"

// --- 6 ---
	strcpy(FUNCTIONLABEL,"inline_rdtscp_32");
#include "counter_test_prolog_32.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdtscp(high,low,aux);
        values32[j] = low;
      }
#include "counter_test_epilog_32.c"

// --- 7 ---
	strcpy(FUNCTIONLABEL,"full_rdtscp");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) values64[j] = full_rdtscp(&chip,&core);
#include "counter_test_epilog.c"

// --- 8 ---
	strcpy(FUNCTIONLABEL,"rdpmc_instructions");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) values64[j] = rdpmc_instructions();
#include "counter_test_epilog.c"

// --- 9 ---
	strcpy(FUNCTIONLABEL,"rdpmc_actual_cycles");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) values64[j] = rdpmc_actual_cycles();
#include "counter_test_epilog.c"

// --- 10 ---
	strcpy(FUNCTIONLABEL,"inline_rdpmc_actual_cycles_64");
	counter = (1<<30)+1; 
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdpmc(high,low,counter);
        values64[j] = ((unsigned long)high<<32) | (unsigned long)low;
      }
#include "counter_test_epilog.c"

// --- 11 ---
	strcpy(FUNCTIONLABEL,"inline_rdpmc_actual_cycles_32");
	counter = (1<<30)+1; 
#include "counter_test_prolog_32.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdpmc(high,low,counter);
        values32[j] = low;
      }
#include "counter_test_epilog_32.c"

// --- 12 ---
	strcpy(FUNCTIONLABEL,"rdpmc");
	counter = 0;
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) values64[j] = rdpmc(counter);
#include "counter_test_epilog.c"

// --- 13 ---
	strcpy(FUNCTIONLABEL,"inline_rdpmc_programmable_64");
	counter = 0; 
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdpmc(high,low,counter);
        values64[j] = ((unsigned long)high<<32) | (unsigned long)low;
      }
#include "counter_test_epilog.c"

// --- 14 ---
	strcpy(FUNCTIONLABEL,"inline_rdpmc_programmable_32");
	counter = 0; 
#include "counter_test_prolog_32.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdpmc(high,low,counter);
        values32[j] = low;
      }
#include "counter_test_epilog_32.c"

// --- 15 ---
	strcpy(FUNCTIONLABEL,"rdpmc_reference_cycles");
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) values64[j] = rdpmc_reference_cycles();
#include "counter_test_epilog.c"

// --- 16 ---
	strcpy(FUNCTIONLABEL,"inline_rdpmc_reference_cycles_64");
	counter = (1<<30)+2; 
#include "counter_test_prolog.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdpmc(high,low,counter);
        values64[j] = ((unsigned long)high<<32) | (unsigned long)low;
      }
#include "counter_test_epilog.c"

// --- 17 ---
	strcpy(FUNCTIONLABEL,"inline_rdpmc_reference_cycles_32");
	counter = (1<<30)+2; 
#include "counter_test_prolog_32.c"
	for (j=0; j<NSAMPLES; j++) {
        inline_rdpmc(high,low,counter);
        values32[j] = low;
      }
#include "counter_test_epilog_32.c"

}
