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
#include "dot_graph.h"

// Returns true if the signal was generated by the IVL
// compiler as an output from a constant net object
ivl_net_const_t is_const_local_sig(ivl_signal_t sig){
	ivl_nexus_t 	nexus     = ivl_signal_nex(sig, 0);
	ivl_nexus_ptr_t nexus_ptr = NULL;
	ivl_net_const_t con       = NULL;

	// Check if local signal is connected to a constant
	for(unsigned i = 0; i < ivl_nexus_ptrs(nexus); i++){
		nexus_ptr = ivl_nexus_ptr(nexus, i);
		if ((con = ivl_nexus_ptr_con(nexus_ptr))){
			return con;
		}
	}

	return con;
}

// *** "Main"/Entry Point *** of iverilog target
int target_design(ivl_design_t des) {

	ivl_scope_t* 	     roots     = 0;    // root scopes of the design
	unsigned 		     num_roots = 0;    // number of root scopes of the design
	Reporter             reporter  = NULL; // reporter object (prints messages)
	DotGraph 		     dg        = NULL; // dot graph object
	vector<ivl_signal_t> signals;   	   // critical signals found in a design

	// Initialize reporter
	reporter = Reporter();
	reporter.init(LAUNCH_MESSAGE);

	// Initialize graphviz dot graph
	dg = DotGraph(ivl_design_flag(des, "-o"));
	dg.init_graph();

	// Get root scopes (top level modules) of design
	reporter.print_message(SCOPE_EXPANSION_MESSAGE);
	ivl_design_roots(des, &roots, &num_roots);
	reporter.root_scopes(roots, num_roots);

	// // Find all critical signals and dependencies in the design
	// find_critical_sigs(roots, num_roots, critical_sigs, CRITICAL_SIG_REGEX);

	// // Find signal dependencies of critical sigs
	// find_all_signal_dependencies(critical_sigs, dg);

	// Close graphviz dot file
	dg.save_graph();

	// Report total execution time
	reporter.end();

	// Stop timer
	// duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	// printf("\nExecution Time: %f (s)\n", duration);
	// printf("-----------------------------\n");

	return 0;
}
