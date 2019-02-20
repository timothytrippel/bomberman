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
#include "ttb.h"
#include "dot_graph.h"
#include "error.h"
#include "debug.h"

unsigned long process_part_select(ivl_lpm_t lpm, ivl_lpm_type_t lpm_type, ivl_signal_t root_signal, sig_map_t& signals_map, DotGraph dg) {
	// Track number of connections enumerated
	unsigned long num_connections = 0;

	// Device Nexuses
	ivl_nexus_t input_nexus = NULL;
	ivl_nexus_t base_nexus  = NULL;

	// Get input pin (0) nexus for part-select LPM device
	input_nexus = ivl_lpm_data(lpm, LPM_PART_SELECT_INPUT_NEXUS_INDEX);

	// Get base pin (1) nexus for part-select LPM device, which
	// may be NULL if not used is non-constant base is used.
	base_nexus = ivl_lpm_data(lpm, LPM_PART_SELECT_BASE_NEXUS_INDEX);
	if (base_nexus) {
		Error::not_supported_error("non-constant base for LPM part select device.");
	}

	// // Get MSB and LSB of Splice
	// unsigned int msb = ivl_lpm_base(lpm) + ivl_lpm_width(lpm) - 1;
	// unsigned int lsb = ivl_lpm_base(lpm);

	num_connections += propagate_nexus(input_nexus, root_signal, signals_map, dg);

	// // Add Connection
	// if (lpm_type == IVL_LPM_PART_VP) {
	// 	// part select: vector to part (part select in rval)
	// 	return;
	// } else if (lpm_type == IVL_LPM_PART_PV) {
	// 	// part select: part select to vector (part select in lval)
	// 	return;
	// } else {
	// 	Error::unknown_part_select_lpm_type_error(lpm_type);
	// }

	return num_connections;
}

unsigned long propagate_lpm(ivl_lpm_t lpm, ivl_nexus_t root_nexus, ivl_signal_t root_signal, sig_map_t& signals_map, DotGraph dg) {
	// Track number of connections enumerated
	unsigned long num_connections = 0;

	// Get LPM type
	const ivl_lpm_type_t lpm_type = ivl_lpm_type(lpm);

	// Add connections
	switch (lpm_type) {
		
		case IVL_LPM_PART_VP:
			fprintf(stdout, "	 		VP\n");

			// ivl_lpm_q() returns the output nexus. If the 
			// output nexus is NOT the same as the root nexus, 
			// then we do not propagate because this nexus is an 
			// to input to an LPM, not an output.
			if (ivl_lpm_q(lpm) == root_nexus) {
				num_connections += process_part_select(lpm, lpm_type, root_signal, signals_map, dg);	
			}

			break;
		case IVL_LPM_PART_PV:
			fprintf(stdout, "	 		PV\n");
			
			// ivl_lpm_q() returns the output nexus. If the 
			// output nexus is NOT the same as the root nexus, 
			// then we do not propagate because this nexus is an 
			// to input to an LPM, not an output.
			if (ivl_lpm_q(lpm) == root_nexus) {
				num_connections += process_part_select(lpm, lpm_type, root_signal, signals_map, dg);
			}

			break;
		case IVL_LPM_CONCAT:
		case IVL_LPM_CONCATZ: {
			fprintf(stdout, "	 		CONCATZ");
			break;
		}
		default:
			break;
	}

	return num_connections;
}
