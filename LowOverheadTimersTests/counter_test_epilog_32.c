	// Boilerplate code that goes after the sample loop for
	//  tests that store only the low-order 32 bits of each result.
	//
	// 1. Collect final values of fixed-function core counters,
	//    programmable core performance counters, and TSC
	//    to monitor behavior over the entire sample loop.
	// 2. Compute min/max/avg deltas on the individual measurements
	//    taken in the loop (excluding the first delta, which is
	//    sometimes slow).
	// 3. Compute core utilization and average frequency for the
	//    entire sample loop.
	// 4. Print out statistics on the deltas measured within
	//    the loop, along with core utilization and avg frequency.
	// 5. Print out the deltas of the programmable counters
	//    over the entire sample loop.
	//
	// Note that this code makes no attempt to detect or correct
	// for wraparound of the fixed or programmable performance 
	// counters -- either for the entire loop or for the individual
	// measurements within the loop!
	//
	gen_cyc_end = rdpmc_actual_cycles(); 
	gen_ref_end = rdpmc_reference_cycles(); 
	tsc_end = rdtscp(); 
	for (i=0; i<NUM_CORE_COUNTERS; i++) core_counter_end[i] = rdpmc(i); 
	minval = 1UL << 60; 
	maxval = 0; 
	avgval = 0;
	for (j=2; j<NSAMPLES; j++) { 
		count = values32[j]-values32[j-1]; 
		minval = MIN(count,minval); 
		maxval = MAX(count,maxval); 
		avgval += count; 
	} 
	avgval = avgval / (double)(NSAMPLES-2);
	utilization = (double)(corrected_pmc_delta(gen_ref_end,gen_ref_start,core_ctr_width))/(double)(tsc_end-tsc_start);
	avg_ghz = (double)(corrected_pmc_delta(gen_cyc_end,gen_cyc_start,core_ctr_width))/(double)(tsc_end-tsc_start)*nominal_ghz; 
	printf("%s minimum %ld average %f maximum %ld utilization %f avg_ghz %f\n",FUNCTIONLABEL,minval,avgval,maxval,utilization,avg_ghz); 
	printf("COUNTERS: %s core count deltas ",FUNCTIONLABEL);
	for (i=0; i<NUM_CORE_COUNTERS; i++) {
		printf(" %lu",corrected_pmc_delta(core_counter_end[i],core_counter_start[i],core_ctr_width));
	}
	printf("\n");
