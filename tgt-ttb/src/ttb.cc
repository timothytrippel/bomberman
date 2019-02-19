/*
File:        ttb.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <cassert>
#include <cstdio>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"
#include "ttb.h"
#include "reporter.h"
#include "error.h"

void find_signals(ivl_scope_t scope, sig_name_map_t& signals_to_names, sig_map_t& signals_map, DotGraph dg) {
	// Current IVL signal
	ivl_signal_t current_signal;

	// Recurse into sub-modules
	for (unsigned int i = 0; i < ivl_scope_childs(scope); i++){
		find_signals(ivl_scope_child(scope, i), signals_to_names, signals_map, dg);		
	}

	// Scope is a base scope
	unsigned int num_signals = ivl_scope_sigs(scope);
	for (unsigned int i = 0; i < num_signals; i++){
		// Get current signal
		current_signal = ivl_scope_sig(scope, i);

		// Check if signal already exists in map
		Error::check_signal_exists_in_map(signals_map, current_signal);

		// Add signal to graph
		// Ignore local (IVL) generated signals.
		if (!ivl_signal_local(current_signal)) {
			// signal was defined in HDL
			signals_map[current_signal]      = vector<ivl_signal_t>();
			signals_to_names[current_signal] = ivl_signal_name(current_signal);
			dg.add_signal_node(current_signal);
		}
	} 
}

void find_all_signals(ivl_scope_t*    scopes, \
					  unsigned int    num_scopes, \
					  sig_name_map_t& signals_to_names, \
					  sig_map_t&      signals_map, \
					  DotGraph        dg) {

	for (unsigned int i = 0; i < num_scopes; i++){
		find_signals(scopes[i], signals_to_names, signals_map, dg);
	}
}

void add_connection(ivl_signal_t signal, ivl_signal_t connected_signal, sig_map_t& signals_map, DotGraph dg) {
	// Only add connections to signals already in graph.
	// Ignore connections to local (IVL generated) signals.
	if (!ivl_signal_local(connected_signal) && signals_map.count(connected_signal)) {
		if (signals_map.count(connected_signal)) {
			signals_map[signal].push_back(connected_signal);
			dg.add_connection(signal, connected_signal);
		} else {
			Error::connecting_signal_not_in_graph(connected_signal);
		}
	}
}

unsigned long find_all_connections(sig_map_t& signals_map, DotGraph dg) {
	// Create a signals map iterator
	sig_map_t::iterator it = signals_map.begin();

	// Nexus Pointer
	ivl_nexus_ptr_t nexus_ptr;

	// Connected Objects
	ivl_signal_t    connected_signal;
	ivl_net_logic_t connected_logic;
	ivl_lpm_t       connected_lpm;
	ivl_net_const_t connected_constant;

	// Track number of connections enumerated
	unsigned long num_connections = 0;
 
	// Iterate over all signals in adjacency list
	while (it != signals_map.end()) { 	
		ivl_signal_t signal = it->first;

 		// Print signal name -- signal dimensions
		fprintf(stdout, "	%s:\n", ivl_signal_name(signal));

		// Check if signal is arrayed
		// @TODO: support arrayed signals 
		// (i.e. signals with more than one nexus)
		Error::check_signal_not_arrayed(signal);

		// Get signal nexus
		// There is exactly one nexus for each WORD of a signal.
		// Since we only support non-arrayed signals (above), 
		// each signal only has one nexus.
		const ivl_nexus_t root_nexus = ivl_signal_nex(signal, 0);

		// Check Nexus IS NOT NULL
    	assert(root_nexus);

    	// Iterate over Nexus pointers in Nexus
    	for (unsigned int nexus_ind = 0; nexus_ind < ivl_nexus_ptrs(root_nexus); nexus_ind++) {
    		nexus_ptr = ivl_nexus_ptr(root_nexus, nexus_ind);
    		fprintf(stdout, "		Nexus %d", nexus_ind);

    		// Determine type of Nexus
    		if ((connected_signal = ivl_nexus_ptr_sig(nexus_ptr))){
    			// Nexus target object is a SIGNAL
    			fprintf(stdout, "	 -- SIGNAL -- %s\n", ivl_signal_name(connected_signal));	
    			num_connections += propagate_signal(connected_signal, signal, signals_map, dg);
    		} else if ((connected_logic = ivl_nexus_ptr_log(nexus_ptr))) {
    			// Nexus target object is a LOG	
    			fprintf(stdout, "	 -- LOGIC -- %x\n", connected_logic);    			
    			num_connections += propagate_logic(connected_logic, signal, signals_map, dg);	
    		} else if ((connected_lpm = ivl_nexus_ptr_lpm(nexus_ptr))) {
    			// Nexus target object is a LPM
    			fprintf(stdout, "	 -- LPM -- %x\n", connected_lpm);
    			num_connections += propagate_lpm(connected_lpm, signal, signals_map, dg);
    		} else if ((connected_constant = ivl_nexus_ptr_con(nexus_ptr))) {
    			// Nexus target object is a CONSTANT
    			fprintf(stdout, "	 -- CONSTANT -- %x\n", connected_constant);
    			num_connections += propagate_constant(connected_constant, signal, signals_map, dg);
    		} else {
    			// Nexus target object is UNKNOWN
    			Error::unknown_nexus_type_error(nexus_ptr);
    		}
    	}

		// Increment the iterator
		it++;
	}

	return num_connections;
}

// *** "Main"/Entry Point *** of iverilog target
int target_design(ivl_design_t des) {
	ivl_scope_t*   roots     = 0;    	// root scopes of the design
	unsigned       num_roots = 0;    	// number of root scopes of the design
	unsigned long  num_connections = 0; // number of connections enumerated in design
	Reporter       reporter;         	// reporter object (prints messages)
	DotGraph       dg;               	// dot graph object
	sig_name_map_t signals_to_names; 	// signal graph (adjacency list)
	sig_map_t      signals_map;         // signal graph (adjacency list)

	// Initialize reporter checking objects
	reporter = Reporter();
	reporter.init(LAUNCH_MESSAGE);

	// Initialize graphviz dot graph
	dg = DotGraph(ivl_design_flag(des, "-o"));
	dg.init_graph();

	// Get root scopes (top level modules) of design
	reporter.print_message(SCOPE_EXPANSION_MESSAGE);
	ivl_design_roots(des, &roots, &num_roots);
	Error::check_scope_types(roots, num_roots);
	reporter.root_scopes(roots, num_roots);

	// Find all critical signals and dependencies in the design
	reporter.print_message(SIGNAL_ENUM_MESSAGE);
	find_all_signals(roots, num_roots, signals_to_names, signals_map, dg);
	reporter.num_signals(signals_map);
	reporter.signal_names(signals_map);

	// Find signal-to-signal connections
	reporter.print_message(CONNECTION_ENUM_MESSAGE);
	num_connections = find_all_connections(signals_map, dg);
	reporter.num_connections(num_connections);

	// Save dot graph to file
	dg.save_graph();

	// Report total execution time
	reporter.end();

	return 0;
}
