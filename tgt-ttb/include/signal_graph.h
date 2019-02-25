/*
File:        signal_graph.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __SIGNAL_GRAPH_HEADER__
#define __SIGNAL_GRAPH_HEADER__

// Standard Headers
#include <string>

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"

using namespace std;

class SignalGraph {
    public:
        // Constructors
        SignalGraph();
        SignalGraph(const char* dot_graph_fname);

        // Signal Enumeration
        void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);

        // Connection Enumeration
        void add_constant_connection(ivl_signal_t    sink_signal, 
                                     ivl_net_const_t source_constant,
                                     string          ws);

        void add_connection(ivl_signal_t sink_signal, 
                            ivl_signal_t source_signal,  
                            string       ws);
        
        // Helper Functions
        void track_lpm_connection_slice(unsigned int      msb, 
                                        unsigned int      lsb,
                                        slice_node_type_t node_type);

        // Getters
        unsigned long get_num_connections();
        unsigned long get_num_signals();
        sig_map_t     get_signals_map();

        // Dot Graph Management
        void          save_dot_graph();
    private:
        // (Private) Member Variables
        unsigned long     num_connections_; // number of connections enumerated in design
        sig_map_t         signals_map_;     // signal graph (adjacency list)
        DotGraph          dg_;              // dot graph object
        vector<SliceInfo> signal_slices_;   // signal slice information stack
        
        // Signal Enumeration
        void find_signals(ivl_scope_t scope);
}; 

#endif
