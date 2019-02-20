/*
File:        debug.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <cstdio>

// TTB Headers
#include "debug.h"
#include "error.h"

void Debug::print_nexus_ptrs(ivl_nexus_t nexus) {
	// Nexus Pointer
	ivl_nexus_ptr_t nexus_ptr;

	// Connected Objects
	ivl_signal_t    connected_signal;
	ivl_net_logic_t connected_logic;
	ivl_lpm_t       connected_lpm;
	ivl_net_const_t connected_constant;

	// Print number of Nexus Ptrs
	fprintf(stdout, "Num. nexus ptrs: %d\n", ivl_nexus_ptrs(nexus));

	// Iterate over Nexus pointers in Nexus
	for (unsigned int nexus_ind = 0; nexus_ind < ivl_nexus_ptrs(nexus); nexus_ind++) {
		nexus_ptr = ivl_nexus_ptr(nexus, nexus_ind);
		fprintf(stdout, "	Nexus Ptr %d is a ", nexus_ind);

		// Determine type of Nexus
		if ((connected_signal = ivl_nexus_ptr_sig(nexus_ptr))){
			// Nexus target object is a SIGNAL
			fprintf(stdout, "SIGNAL -- %s\n", ivl_signal_name(connected_signal));	
		} else if ((connected_logic = ivl_nexus_ptr_log(nexus_ptr))) {
			// Nexus target object is a LOG	
			fprintf(stdout, "LOGIC -- %x\n", connected_logic);    			
		} else if ((connected_lpm = ivl_nexus_ptr_lpm(nexus_ptr))) {
			// Nexus target object is a LPM
			fprintf(stdout, "LPM -- %x\n", connected_lpm);
		} else if ((connected_constant = ivl_nexus_ptr_con(nexus_ptr))) {
			// Nexus target object is a CONSTANT
			fprintf(stdout, "CONSTANT -- %x\n", connected_constant);
		} else {
			// Nexus target object is UNKNOWN
			Error::unknown_nexus_type_error(nexus_ptr);
		}
	}
}
