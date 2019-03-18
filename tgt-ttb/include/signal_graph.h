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
        
        // Destructors
        ~SignalGraph();

        // Signal Enumeration
        void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);

        // Connection Enumeration
        void add_connection(
            Signal* sink_signal,
            Signal* source_signal,
            string  ws);

        void process_local_connections(string ws);

        // Graph Stats Getters
        unsigned long get_num_connections()   const;
        unsigned long get_num_signals()       const;
        unsigned long get_num_local_signals() const;
        unsigned long get_num_constants()     const;

        // Signals Map Getters
        sig_map_t get_signals_map()              const;
        bool      in_signals_map(Signal* signal) const;
        Signal*   get_signal_from_ivl_signal(ivl_signal_t ivl_signal);
        
        // Source Signals Queue Getters
        unsigned long   get_num_source_signals()              const;
        signals_q_t     get_source_signals_queue()            const;
        Signal*         get_source_signal(unsigned int index) const;
        Signal*         pop_from_source_signals_queue();
        void            pop_from_source_signals_queue(unsigned int num_nodes);

        // Enumeration Depth Queue Getters
        unsigned long get_scope_depth() const;
        unsigned int  pop_from_num_signals_at_depth_queue();

        // Slice Tracking Queues Getters
        unsigned int   get_num_source_slices() const;
        unsigned int   get_num_sink_slices()   const;
        signal_slice_t pop_from_source_slices_queue(Signal* source_signal);
        signal_slice_t pop_from_sink_slices_queue(Signal* sink_signal);
        
        // Connection Tracking Getters
        bool check_if_connection_exists(
            Signal*     sink_signal, 
            Connection* new_conn);

        bool check_if_local_connection_exists(
            Signal*     sink_signal, 
            Connection* new_conn);

        bool check_if_ignore_signal(Signal* signal) const;

        bool check_if_ignore_signal(ivl_signal_t signal) const;

        string_map_t get_signals_to_ignore() const;

        // Source Signals Queue Setters
        void push_to_source_signals_queue(Signal* source_node, string ws);

        // Enumeration Depth Queue Setters
        void push_to_num_signals_at_depth_queue(unsigned int num_signals);

        // Slice Tracking Queues Setters
        void track_source_slice(unsigned int msb, 
                                unsigned int lsb,
                                string       ws);

        void track_sink_slice(unsigned int msb, 
                              unsigned int lsb,
                              string       ws);

        void erase_index_from_sink_slices(unsigned int index);

        // Connection Tracking Setters
        void track_local_signal_connection(Signal* sink_signal, Signal* source_signal, string ws);
        void add_signal_to_ignore(string signal_basename);
        void load_signals_to_ignore(string file_path);

        // Dot Graph Management
        void save_dot_graph();
        
    private:
        unsigned long          num_signals_;           // number of signals enumerated in design
        unsigned long          num_local_signals_;     // number of local signals optimized aways
        unsigned long          num_constants_;         // number of constants enumerated in design
        unsigned long          num_connections_;       // number of connections enumerated in design
        unsigned long          num_local_connections_; // number of local connections to be processed
        sig_map_t              signals_map_;           // IVL signal to Signal object map
        DotGraph               dg_;                    // dot graph object
        signals_q_t            source_signals_;        // source signal queue
        vector<unsigned int>   num_signals_at_depth_;  // back is num source nodes at current depth
        vector<signal_slice_t> source_slices_;         // source signal slice information stack
        vector<signal_slice_t> sink_slices_;           // sink signal slice information stack
        conn_map_t             connections_map_;       // sink signal to connections map
        conn_map_t             local_connections_map_; // local signal connections to be processed
        string_map_t           signals_to_ignore_;     // names of signals to ignore

        // Signal Enumeration
        void find_signals(ivl_scope_t scope);
}; 

#endif
