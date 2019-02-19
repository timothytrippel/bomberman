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

unsigned long propagate_lpm(ivl_lpm_t lpm_device, ivl_signal_t signal, sig_map_t& signals_map, DotGraph dg){
	// Track number of connections enumerated
	unsigned long num_connections = 0;
	
	return num_connections;
}
