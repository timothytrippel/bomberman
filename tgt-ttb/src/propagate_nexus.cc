/*
File:        propagate_nexus.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes a root signal through a nexus. A nexus
is how IVL connects all data-structures (signals, logic-devices,
LPMs, and constant ). Think of a nexus like a pin, connecting
nets and devices together. Each nexus is associated with a 
single IVL data-structure, where data-structures can have
multiple nexis associated with them. Nexi contain muliplte 
nexus pointers that point to other nexus. Every nexus contains 
a nexus pointer to itself, and the rest are pointers to all the 
IVL data-structures it connects. 
*/

// Standard Headers

// TTB Headers
#include "ttb.h"
#include "error.h"

void SignalGraph::propagate_nexus(ivl_nexus_t nexus, ivl_signal_t root_signal) {
	// Nexus Pointer
	ivl_nexus_ptr_t nexus_ptr = NULL;

	// Connected Objects
	ivl_nexus_ptr_t connected_nexus_ptr = NULL;
	ivl_signal_t    connected_signal    = NULL;
	ivl_net_logic_t connected_logic     = NULL;
	ivl_lpm_t       connected_lpm       = NULL;
	ivl_net_const_t connected_constant  = NULL;

	// Iterate over Nexus pointers in Nexus
	for (unsigned int nexus_ind = 0; nexus_ind < ivl_nexus_ptrs(nexus); nexus_ind++) {
		nexus_ptr = ivl_nexus_ptr(nexus, nexus_ind);
		fprintf(stdout, "		Nexus %d", nexus_ind);

		// Determine type of Nexus
		if ((connected_signal = ivl_nexus_ptr_sig(nexus_ptr))){
			// Nexus target object is a SIGNAL
			fprintf(stdout, "	 -- SIGNAL -- %s\n", ivl_signal_name(connected_signal));	
			// propagate_signal(connected_signal, root_signal);

			// BASE-CASE:
			// If connected signal and signal the same, 
			// IGNORE, probably a module hookup
			// @TODO: investigate this
			// Ignore connections to local (IVL generated) signals.
			if (connected_signal != root_signal && !ivl_signal_local(connected_signal)){
				add_connection(root_signal, connected_signal, nexus);
				num_connections_++;
			}
		} else if ((connected_logic = ivl_nexus_ptr_log(nexus_ptr))) {
			// Nexus target object is a LOGIC
			fprintf(stdout, "	 -- LOGIC -- %s\n", get_logic_type_as_string(connected_logic));
			propagate_logic(connected_logic, nexus, root_signal);
		} else if ((connected_lpm = ivl_nexus_ptr_lpm(nexus_ptr))) {
			// Nexus target object is a LPM
			fprintf(stdout, "	 -- LPM -- %s\n", get_lpm_type_as_string(connected_lpm));
			propagate_lpm(connected_lpm, nexus, root_signal);
		} else if ((connected_constant = ivl_nexus_ptr_con(nexus_ptr))) {
			// Nexus target object is a CONSTANT
			fprintf(stdout, "	 -- CONSTANT -- %x\n", connected_constant);
			// num_connections += propagate_constant(connected_constant);
		} else {
			// Nexus target object is UNKNOWN
			Error::unknown_nexus_type_error(nexus_ptr);
		}
	}
}
