/*
File:        propagate_logic.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signals connected to a LOGIC device.
The output of every LOGIC device is connected to pin 0, or 
the nexus at index 0. Therefore, we should only propagate signals
connceted to this nexus, since we want to create a directed graph.
If the output nexus (i.e. nexus at index 0) is NOT the same as
the current signal nexus, then we should NOT propagate it as this
nexus is connected to the INPUT of a LOGIC device.
*/

// Standard Headers
#include <cassert>
#include <cstdio>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"
#include "ttb.h"
#include "error.h"

unsigned long propagate_logic(ivl_net_logic_t logic_device, ivl_signal_t signal, sig_map_t& signals_map, DotGraph dg) {
	// Track number of connections enumerated
	unsigned long num_connections = 0;
	
	// Get signal nexus
	const ivl_nexus_t signal_nexus = ivl_signal_nex(signal, 0);

	// LOGIC device pin nexus
	ivl_nexus_t pin_nexus = NULL;

	// Connections
	ivl_nexus_ptr_t connected_nexus_ptr = NULL;
	ivl_signal_t    connected_signal    = NULL;
	ivl_net_logic_t connected_logic     = NULL;
	ivl_lpm_t       connected_lpm       = NULL;
	ivl_net_const_t connected_constant  = NULL;

	// Get number of pins on LOGIC device.
	// Each LOGIC device pin is a Nexus.
	unsigned int num_pins = ivl_logic_pins(logic_device);

	// Pin 0 is the output of every logic device.		
	if (ivl_logic_pin(logic_device, 0) == signal_nexus) {
		
		// Iterate over all input pins (nexuses) of LOGIC device.
		// Pin 0 is the output, so start with pin 1.
		for (unsigned int i = 1; i < num_pins; i++) {
			pin_nexus = ivl_logic_pin(logic_device, i);

			for (unsigned int j = 0; j < ivl_nexus_ptrs(pin_nexus); j++) {
				connected_nexus_ptr = ivl_nexus_ptr(pin_nexus, j);

				// Do not process nexus pointer to self (logic device)
				if (ivl_nexus_ptr_log(connected_nexus_ptr) != logic_device) {

					// Only support input pins connected to SIGNAL and CONSTANT nexuses
					if ((connected_signal = ivl_nexus_ptr_sig(connected_nexus_ptr))) {
						fprintf(stdout, "	 		SIGNAL -- %s\n", ivl_signal_name(connected_signal));
						add_connection(signal, connected_signal, signals_map, dg);
						num_connections++;
					} else if ((connected_constant = ivl_nexus_ptr_con(connected_nexus_ptr))) {
						fprintf(stdout, "	 		CONSTANT -- %x\n", connected_constant);
						Error::not_supported_error("direct CONSTANT to LOGIC connection.");
					} else if ((connected_logic = ivl_nexus_ptr_log(connected_nexus_ptr))) {
						// @TODO: Unsupported
						fprintf(stderr, "	 		LOGIC -- %x\n", connected_logic);
						Error::not_supported_error("direct LOGIC to LOGIC connection.");
					} else if ((connected_lpm = ivl_nexus_ptr_lpm(connected_nexus_ptr))) {
						// @TODO: Unsupported
						fprintf(stderr, "	 		LPM -- %x\n", connected_lpm);
						Error::not_supported_error("direct LPM to LOGIC connection.");
					} else {
						Error::unknown_logic_nexus_type_error(connected_nexus_ptr);
					}
				}
			}
		}
	}	

	return num_connections;
}