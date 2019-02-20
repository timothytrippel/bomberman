// /*
// File:        propagate_signal.cc
// Author:      Timothy Trippel
// Affiliation: MIT Lincoln Laboratory
// Description:

// This function propagtes signals connected to other SIGNALS.
// This usually happens on module hookups. In order to create
// a directed graph, we need to know what signals are inputs/
// outputs of module so we know the direction of information flow.
// @TODO consider cases of module INPUTS/OUTPUTS.
// */

// // Standard Headers
// #include <cassert>
// #include <cstdio>

// // TTB Headers
// #include "ttb_typedefs.h"
// #include "ttb.h"
// #include "dot_graph.h"
// #include "error.h"
// #include "debug.h"

// unsigned long propagate_signal(ivl_signal_t connected_signal, ivl_signal_t signal, sig_map_t& signals_map, DotGraph dg){
// 	// Track number of connections enumerated
// 	unsigned long num_connections = 0;

// 	// If connected signal and signal the same, 
// 	// IGNORE, probably a module hookup
// 	// @TODO: investigate this
// 	// Ignore connections to local (IVL generated) signals.
// 	if (connected_signal != signal && !ivl_signal_local(connected_signal)){
// 		add_connection(signal, connected_signal, signals_map, dg);
// 		num_connections++;
// 	}

// 	return num_connections;
// }
