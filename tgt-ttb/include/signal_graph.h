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
        SignalGraph(cmd_args_map_t* cmd_args);
        
        // Destructors
        ~SignalGraph();

        // Signal Enumeration
        void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);
        bool check_if_ignore_signal_basename(Signal* signal)      const;
        bool check_if_ignore_signal_fullname(Signal* signal)      const;
        bool check_if_ignore_signal_basename(ivl_signal_t signal) ;
        bool check_if_ignore_signal_fullname(ivl_signal_t signal) ;
        
        // Connection Enumeration
        bool check_if_connection_exists(
            Signal*     sink_signal, 
            Connection* new_conn);

        bool check_if_local_connection_exists(
            Signal*     sink_signal, 
            Connection* new_conn);

        bool add_connection(
            Signal*        sink_signal,
            Signal*        source_signal,
            signal_slice_t sink_slice,
            signal_slice_t source_slice,
            string         ws);

        void track_local_signal_connection(
            Signal*        sink_signal, 
            Signal*        source_signal,
            signal_slice_t sink_slice,
            signal_slice_t source_slice,
            string         ws);

        void process_local_connections(string ws);

        // Stats Counters
        unsigned long get_num_connections()   const;
        unsigned long get_num_signals()       const;
        unsigned long get_num_local_signals() const;
        unsigned long get_num_constants()     const;

        // Signal Map
        sig_map_t get_signals_map()              const;
        bool      in_signals_map(Signal* signal) const;
        Signal*   get_signal_from_ivl_signal(ivl_signal_t ivl_signal);

        // Dot Graph Management
        void write_signals_to_dot_graph();
        void save_dot_graph();
        
    private:
        // Stats Counters
        unsigned long num_signals_;           // number of signals enumerated in design
        unsigned long num_signals_ignored_;   // number of signals ignored
        unsigned long num_local_signals_;     // number of local signals optimized away
        unsigned long num_constants_;         // number of constants enumerated in design
        unsigned long num_connections_;       // number of connections enumerated in design
        unsigned long num_local_connections_; // number of local connections to be processed

        // Graph Data
        DotGraph*  dg_;                    // dot graph object
        sig_map_t  signals_map_;           // IVL signal to Signal object map
        conn_map_t connections_map_;       // sink signal to connections map
        conn_map_t local_connections_map_; // local signal connections to be processed
        string_map_t signals_to_ignore_;   // names of signals to ignore

        // Signal Enumeration
        void find_signals(ivl_scope_t scope);
        void add_signal(ivl_signal_t signal);

        // Config Loading
        void process_cmd_line_args(cmd_args_map_t* cmd_args);
        void load_signals_to_ignore(string file_path);
        void add_signal_to_ignore(string signal_basename);

        // Destructor Helpers
        void delete_signals_map();
        void delete_connections_queue(conn_q_t* conn_q);
        void delete_connections_map();
}; 

#endif
