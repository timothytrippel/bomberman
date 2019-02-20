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
#include "ttb.h"
#include "dot_graph.h"
#include "error.h"
#include "debug.h"

unsigned long propagate_logic(ivl_net_logic_t logic_device, ivl_nexus_t root_nexus, ivl_signal_t root_signal, sig_map_t& signals_map, DotGraph dg) {
	// Track number of connections enumerated
	unsigned long num_connections = 0;

	// LOGIC device pin nexus
	ivl_nexus_t pin_nexus = NULL;

	// Get number of pins on LOGIC device.
	// Each LOGIC device pin is a Nexus.
	unsigned int num_pins = ivl_logic_pins(logic_device);
	
	// Pin 0 is the output. If the (root) nexus, is not the
	// same as the output nexus, then we do not propagate,
	// because this root_nexus is an input not an output.
	if (ivl_logic_pin(logic_device, 0) == root_nexus) {
	
		// Iterate over all input pins (nexuses) of LOGIC device.
		// Pin 0 is the output, so start with pin 1.
		for (unsigned int i = 1; i < num_pins; i++) {
			pin_nexus = ivl_logic_pin(logic_device, i);

			// Propagate the nexus
			num_connections += propagate_nexus(pin_nexus, root_signal, signals_map, dg);
		}
	}

	return num_connections;
}
