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
        void add_constant_connection(ivl_signal_t sink_signal, 
                                     node_t       source_node,
                                     string       ws);

        void add_signal_connection(ivl_signal_t sink_signal, 
                                   ivl_signal_t source_signal,  
                                   string       ws);

        void add_connection(ivl_signal_t sink_signal,
                            node_t       source_node, 
                            string       ws);

        // Helper Functions
        void track_connection_slice(unsigned int      msb, 
                                    unsigned int      lsb,
                                    node_slice_type_t node_slice_type,
                                    string            ws);

        // Getters
        //      Graph Stats
        unsigned long get_num_connections();
        unsigned long get_num_signals();
        unsigned long get_num_constants();

        //      Signals Map
        sig_map_t     get_signals_map();
        
        //      Source Nodes Queue
        unsigned long get_num_source_nodes();
        node_q_t      get_source_nodes_queue();
        string        get_node_fullname(node_t node);
        node_t        pop_from_source_nodes_queue();

        //      Slice Tracking Queues
        unsigned int  get_num_source_slices();
        unsigned int  get_num_sink_slices();
        node_slice_t  pop_from_source_slices_queue(ivl_signal_t source_signal);
        node_slice_t  pop_from_sink_slices_queue(ivl_signal_t sink_signal);
        
        // Setters
        void push_to_source_nodes_queue(node_t source_node, string ws);

        // Dot Graph Management
        void save_dot_graph();
    private:
        // (Private) Member Variables
        unsigned long        num_constants_;   // number of constants enumerated in design
        unsigned long        num_connections_; // number of connections enumerated in design
        sig_map_t            signals_map_;     // signal graph (adjacency list)
        DotGraph             dg_;              // dot graph object
        node_q_t             source_nodes_;    // source signal queue
        vector<node_slice_t> source_slices_;   // source signal slice information stack
        vector<node_slice_t> sink_slices_;     // sink signal slice information stack
        
        // Signal Enumeration
        void find_signals(ivl_scope_t scope);
}; 

#endif
