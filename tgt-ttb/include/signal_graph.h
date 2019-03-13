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

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <string>
#include <vector>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "signal.h"
#include "connection.h"
#include "dot_graph.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ----------------------- Signal Graph -----------------------
// ------------------------------------------------------------

class SignalGraph {
    public:
        // Constructors
        SignalGraph();
        SignalGraph(const char* dot_graph_fname);

        // Signal Enumeration
        void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);

        // Connection Enumeration
        void add_constant_connection(Signal sink_signal, 
                                     Signal source_node,
                                     string ws);

        void add_signal_connection(Signal sink_signal, 
                                   Signal source_signal,  
                                   string ws);

        void add_connection(Signal sink_signal,
                            Signal source_signal,
                            string ws);

        // Graph Stats Getters
        unsigned long get_num_connections() const;
        unsigned long get_num_signals()     const;
        unsigned long get_num_constants()   const;

        // Signals Map Getter
        sig_map_t     get_signals_map() const;
        
        // Source Signals Queue Getters
        unsigned long  get_num_source_signals()              const;
        vector<Signal> get_source_signals_queue()            const;
        Signal         get_source_signal(unsigned int index) const;
        Signal         pop_from_source_signals_queue();
        void           pop_from_source_signals_queue(unsigned int num_nodes);

        // Enumeration Depth Queue Getters
        unsigned long get_scope_depth() const;
        unsigned int  pop_from_num_signals_at_depth_queue();

        // Slice Tracking Queues Getters
        unsigned int   get_num_source_slices() const;
        unsigned int   get_num_sink_slices()   const;
        signal_slice_t pop_from_source_slices_queue(Signal source_signal);
        signal_slice_t pop_from_sink_slices_queue(Signal sink_signal);
        
        // Source Nodes Queue Setters
        void push_to_source_signals_queue(Signal source_node, string ws);

        // Enumeration Depth Queue Setters
        void push_to_num_signals_at_depth_queue(unsigned int num_signals);

        // Slice Tracking Queues Setters
        void track_source_slice(unsigned int msb, 
                                unsigned int lsb,
                                string       ws);

        void track_sink_slice(unsigned int msb, 
                              unsigned int lsb,
                              string       ws);

        // Dot Graph Management
        void save_dot_graph();
    
    private:
        unsigned long          num_constants_;        // number of constants enumerated in design
        unsigned long          num_connections_;      // number of connections enumerated in design
        sig_map_t              signals_map_;          // IVL signal to Signal object map
        DotGraph               dg_;                   // dot graph object
        vector<Signal>         source_signals_;       // source signal queue
        vector<unsigned int>   num_signals_at_depth_; // back is num source nodes at current depth
        vector<signal_slice_t> source_slices_;        // source signal slice information stack
        vector<signal_slice_t> sink_slices_;          // sink signal slice information stack
        
        // Signal Enumeration
        void find_signals(ivl_scope_t scope);
}; 

#endif
