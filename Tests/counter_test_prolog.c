	// Boilerplate code that goes before the sample loop for
	//  tests that store the full 64 bits of each result.
	//
	// 1. Initialize the result array so that the corresponding
	//    cache lines will be dirty in the L1 Data Cache.
	//    (The (j>>24) construct is used to inhibit replacement
	//    of this loop by a call to "memset()", which may contain
	//    unwanted SIMD instructions that change the processor
	//    power level.)
	// 2. Collect initial values of fixed-function core counters,
	//    programmable core performance counters, and TSC
	//    to monitor behavior over the entire sample loop.

	for (j=0; j<NSAMPLES; j++) values64[j] = (j>>24); 
	for (i=0; i<4; i++) pgm_counter_start[i] = rdpmc(i); 
	gen_cyc_start = rdpmc_actual_cycles(); 
	gen_ref_start = rdpmc_reference_cycles(); 
	tsc_start = rdtscp(); 

