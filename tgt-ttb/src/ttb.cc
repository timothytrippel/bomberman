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
#include "ttb.h"
#include "reporter.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------
SignalGraph::SignalGraph(): \
	num_connections_(0), \
	signals_names_(), \
	signals_map_(), \
	dg_() {}

SignalGraph::SignalGraph(const char* dot_graph_fname) {
	// Initialize Connection Counter
	num_connections_ = 0;

	// Initialize DotGraph
	dg_ = DotGraph(dot_graph_fname);
	dg_.init_graph();
}

// ----------------------------------------------------------------------------------
// ------------------------------- Getters ------------------------------------------
// ----------------------------------------------------------------------------------
unsigned long SignalGraph::get_num_connections() {
	return num_connections_;
}

unsigned long SignalGraph::get_num_signals() {
	return signals_map_.size();
}

vector<const char*> SignalGraph::get_signals_names() {
	return signals_names_;
}

// ----------------------------------------------------------------------------------
// ------------------------------- Dot Graph Management -----------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::save_dot_graph() {
	dg_.save_graph();
}

void SignalGraph::add_connection(ivl_signal_t root_signal, 
                                 ivl_signal_t connected_signal, 
                                 ivl_nexus_t  nexus) {

	// Only add connections to signals already in graph.
	if (signals_map_.count(connected_signal)) {
		signals_map_[root_signal].push_back(connected_signal);

		// Check if connection is sliced
		if (signal_slices_.size()) {
			SliceInfo signal_slice = signal_slices_.back();

			// Check that slice is to be applied at this nexus
			if (signal_slice.nexus == nexus) {
				
				// add sliced connection
				if (signal_slice.slice_root) {
					// sliced root signal
					dg_.add_sliced_connection(root_signal, \
											  signal_slice.msb, \
											  signal_slice.lsb, \
											  connected_signal, \
											  ivl_signal_packed_msb(connected_signal, 0), \
											  ivl_signal_packed_lsb(connected_signal, 0));
				} else {
					// sliced connected signal
					dg_.add_sliced_connection(root_signal, \
											  ivl_signal_packed_msb(root_signal, 0), \
											  ivl_signal_packed_lsb(root_signal, 0), \
											  connected_signal, \
											  signal_slice.msb, \
											  signal_slice.lsb);
				}

				// pop slice info from stack
				signal_slices_.pop_back();
			}
		} else {
			// full connection
			dg_.add_connection(root_signal, connected_signal);
		}
	} else {
		Error::connecting_signal_not_in_graph(connected_signal);
	}
}

// ----------------------------------------------------------------------------------
// ------------------------------- Signal Enumeration -------------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes) {
	for (unsigned int i = 0; i < num_scopes; i++){
		find_signals(scopes[i]);
	}
}

void SignalGraph::find_signals(ivl_scope_t scope) {
	// Current IVL signal
	ivl_signal_t current_signal;

	// Recurse into sub-modules
	for (unsigned int i = 0; i < ivl_scope_childs(scope); i++){
		find_signals(ivl_scope_child(scope, i));		
	}

	// Scope is a base scope
	unsigned int num_signals = ivl_scope_sigs(scope);
	for (unsigned int i = 0; i < num_signals; i++){
		// Get current signal
		current_signal = ivl_scope_sig(scope, i);

		// Check if signal already exists in map
		Error::check_signal_exists_in_map(signals_map_, current_signal);

		// Add signal to graph
		// Ignore local (IVL) generated signals.
		if (!ivl_signal_local(current_signal)) {
			// signal was defined in HDL
			signals_map_[current_signal] = vector<ivl_signal_t>();
			signals_names_.push_back(ivl_signal_name(current_signal));
			dg_.add_signal_node(current_signal);
		}
	} 
}

// ----------------------------------------------------------------------------------
// ------------------------------- Connection Enumeration ---------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::find_all_connections() {
	// Create a signals map iterator
	sig_map_t::iterator it = signals_map_.begin();
 
	// Iterate over all signals in adjacency list
	while (it != signals_map_.end()) { 	
		ivl_signal_t root_signal = it->first;

 		// Print signal name -- signal dimensions
		fprintf(stdout, "	%s:\n", ivl_signal_name(root_signal));

		// Check if signal is arrayed
		// @TODO: support arrayed signals 
		// (i.e. signals with more than one nexus)
		Error::check_signal_not_arrayed(root_signal);

		// Get signal nexus
		// There is exactly one nexus for each WORD of a signal.
		// Since we only support non-arrayed signals (above), 
		// each signal only has one nexus.
		const ivl_nexus_t root_nexus = ivl_signal_nex(root_signal, 0);

		// Check Nexus IS NOT NULL
		assert(root_nexus);

		// Propagate the nexus
		propagate_nexus(root_nexus, root_signal);

		// Increment the iterator
		it++;
	}
}

// ----------------------------------------------------------------------------------
// ------------------------ IVL Target Entry Point "main" ---------------------------
// ----------------------------------------------------------------------------------
int target_design(ivl_design_t des) {
	ivl_scope_t*   roots     = 0;    	// root scopes of the design
	unsigned       num_roots = 0;    	// number of root scopes of the design
	Reporter       reporter;         	// reporter object (prints messages)
	SignalGraph    sg;                  // signal graph object

	// Initialize reporter checking objects
	reporter = Reporter();
	reporter.init(LAUNCH_MESSAGE);

	// Initialize SignalGraph
	sg = SignalGraph(ivl_design_flag(des, "-o"));
	
	// Get root scopes (top level modules) of design
	reporter.print_message(SCOPE_EXPANSION_MESSAGE);
	ivl_design_roots(des, &roots, &num_roots);
	Error::check_scope_types(roots, num_roots);
	reporter.root_scopes(roots, num_roots);

	// Find all critical signals and dependencies in the design
	reporter.print_message(SIGNAL_ENUM_MESSAGE);
	sg.find_all_signals(roots, num_roots);
	reporter.num_signals(sg.get_num_signals());
	reporter.signal_names(sg.get_signals_names());

	// Find signal-to-signal connections
	reporter.print_message(CONNECTION_ENUM_MESSAGE);
	sg.find_all_connections();
	reporter.num_connections(sg.get_num_connections());

	// Save dot graph to file
	sg.save_dot_graph();

	// Report total execution time
	reporter.end();

	return 0;
}
