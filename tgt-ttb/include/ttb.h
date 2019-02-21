/*
File:        ttb.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_HEADER__
#define __TTB_HEADER__

using namespace std;

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"

// Define Indexes
#define LPM_PART_SELECT_INPUT_NEXUS_INDEX 0
#define LPM_PART_SELECT_BASE_NEXUS_INDEX  1

// Debugging Switches
#define DEBUG_PRINTS false

class SignalGraph {
	public:
		SignalGraph();
		SignalGraph(const char* dot_graph_fname);
		void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);
		void find_all_connections();
		unsigned long get_num_connections();
		unsigned long get_num_signals();
		vector<const char*> get_signals_names();
		void save_dot_graph();
	private:
		unsigned long       num_connections_; // number of connections enumerated in design
		vector<const char*> signals_names_;   // signal graph (adjacency list)
		sig_map_t           signals_map_;     // signal graph (adjacency list)
		DotGraph            dg_;              // dot graph object
		vector<SliceInfo>   signal_slices_;
		void find_signals(ivl_scope_t scope);
		void add_connection(ivl_signal_t root_signal, ivl_signal_t connected_signal, ivl_nexus_t nexus);
		void propagate_nexus(ivl_nexus_t nexus, ivl_signal_t root_signal);
		void propagate_logic(ivl_net_logic_t logic, ivl_nexus_t root_nexus, ivl_signal_t root_signal);
		const char* get_logic_type_as_string(ivl_net_logic_t logic);
		void propagate_lpm(ivl_lpm_t lpm, ivl_nexus_t root_nexus, ivl_signal_t root_signal);
		const char* get_lpm_type_as_string(ivl_lpm_t lpm);
		void process_lpm_part_select(ivl_lpm_t lpm, ivl_nexus_t root_nexus, ivl_signal_t root_signal);
};	

#endif
