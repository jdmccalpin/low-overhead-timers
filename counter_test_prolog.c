	// code that goes before the counter test loop
    for (j=0; j<NSAMPLES; j++) values64[j] = 0; 
    for (i=0; i<4; i++) pgm_counter_start[i] = rdpmc(i); 
    gen_cyc_start = rdpmc_actual_cycles(); 
    gen_ref_start = rdpmc_reference_cycles(); 
    tsc_start = rdtscp(); 

