	// code that goes after the sample loop
    gen_cyc_end = rdpmc_actual_cycles(); 
    gen_ref_end = rdpmc_reference_cycles(); 
    tsc_end = rdtscp(); 
    for (i=0; i<4; i++) pgm_counter_end[i] = rdpmc(i); 
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
	utilization = (double)(gen_ref_end-gen_ref_start)/(double)(tsc_end-tsc_start);
    avg_ghz = (double)(gen_cyc_end-gen_cyc_start)/(double)(tsc_end-tsc_start)*nominal_ghz; 
    printf("%s minimum %ld average %f maximum %ld utilization %f avg_ghz %f\n",FUNCTIONLABEL,minval,avgval,maxval,utilization,avg_ghz); 
    printf("COUNTERS: %s core count deltas %lu %lu %lu %lu\n", FUNCTIONLABEL,
			pgm_counter_end[0]-pgm_counter_start[0], pgm_counter_end[1]-pgm_counter_start[1],
			pgm_counter_end[2]-pgm_counter_start[2], pgm_counter_end[3]-pgm_counter_start[3]); 
