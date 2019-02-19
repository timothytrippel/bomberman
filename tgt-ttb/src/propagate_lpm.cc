 /*
File:        propagate_lpm.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signals connected to an LPM.
*/

// Standard Headers
#include <cassert>
#include <cstdio>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"
#include "ttb.h"
#include "error.h"

void process_part_select(ivl_lpm_t lpm){
	// Get input pin (0) nexus for part-select LPM device
	ivl_nexus_t input_pin_nexus = ivl_lpm_data(lpm, LPM_PART_SELECT_INPUT_PIN_INDEX);

	// Get output pin (0) nexus for part-select LPM device
	ivl_nexus_t base_pin_nexus = ivl_lpm_data(lpm, LPM_PART_SELECT_BASE_PIN_INDEX);

	// Get output pin (0) nexus for part-select LPM device
	ivl_nexus_t output_pin_nexus = ivl_lpm_q(lpm);
}

unsigned long propagate_lpm(ivl_lpm_t lpm, ivl_signal_t signal, sig_map_t& signals_map, DotGraph dg){
	// Track number of connections enumerated
	unsigned long num_connections = 0;

	// Connections
	ivl_nexus_ptr_t connected_nexus_ptr = NULL;
	ivl_signal_t    connected_signal    = NULL;
	ivl_net_logic_t connected_logic     = NULL;
	ivl_lpm_t       connected_lpm       = NULL;
	ivl_net_const_t connected_constant  = NULL;

	// Get signal nexus
	const ivl_nexus_t signal_nexus = ivl_signal_nex(signal, 0);

	// Get LPM type
	const ivl_lpm_type_t lpm_type = ivl_lpm_type(lpm);

	// Add connections
	switch (lpm_type) {
		/* part select: vector to part (part select in rval) */
		case IVL_LPM_PART_VP:
			fprintf(stdout, "	 		VP -- %s\n", ivl_signal_name(signal));
			break;
		/* part select: part select to vector (part select in lval) */
		case IVL_LPM_PART_PV:
			fprintf(stdout, "	 		PV -- %s\n", ivl_signal_name(signal));
			break;
		case IVL_LPM_CONCAT:
		case IVL_LPM_CONCATZ: {
			fprintf(stdout, "	 		CONCATZ -- %s\n", ivl_signal_name(signal));
			break;
		}
		default:
			break;
	}

	return num_connections;
}
