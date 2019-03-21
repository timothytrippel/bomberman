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

        // Connection Enumeration
        bool add_connection(
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
        unsigned long get_num_source_signals()                        const;
        signals_q_t   get_source_signals_queue()                      const;
        Signal*       get_source_signal(unsigned int index)           const;
        bool          check_if_clk_signal(ivl_signal_t source_signal) const;
        Signal*       pop_from_source_signals_queue();
        void          pop_from_source_signals_queue(unsigned int num_nodes);

        // Enumeration Depth Queue Getters
        unsigned long get_scope_depth() const;
        unsigned int  pop_from_num_signals_at_depth_queue();

        // Slice Tracking Queues Getters
        unsigned int   get_num_source_slices() const;
        unsigned int   get_num_sink_slices()   const;
        signal_slice_t get_source_slice(Signal* source_signal) const;
        signal_slice_t get_sink_slice(Signal* sink_signal) const;
        
        // Connection Tracking Getters
        bool check_if_connection_exists(
            Signal*     sink_signal, 
            Connection* new_conn);

        bool check_if_local_connection_exists(
            Signal*     sink_signal, 
            Connection* new_conn);

        bool check_if_ignore_signal(Signal* signal)      const;
        bool check_if_ignore_signal(ivl_signal_t signal) const;
        bool check_if_inside_ff_block()                  const;

        // Source Signals Queue Setters
        void push_to_source_signals_queue(Signal* source_node, string ws);

        // Enumeration Depth Queue Setters
        void push_to_num_signals_at_depth_queue(unsigned int num_signals);

        // Slice Tracking Queues Setters
        void track_source_slice(
            unsigned int msb, 
            unsigned int lsb,
            string       ws);

        void track_sink_slice(
            unsigned int msb, 
            unsigned int lsb,
            string       ws);

        void pop_from_source_slices_queue();
        void pop_from_sink_slices_queue();
        void set_track_source_slices_flag();
        void set_track_sink_slices_flag();
        void set_all_slice_tracking_flags();
        void clear_track_source_slices_flag();
        void clear_track_sink_slices_flag();
        void clear_all_slice_tracking_flags();
        void erase_index_from_sink_slices(unsigned int index);

        // Connection Tracking Setters
        void set_inside_ff_block();
        void clear_inside_ff_block();

        void track_local_signal_connection(
            Signal* sink_signal, 
            Signal* source_signal, 
            string  ws);

        // Dot Graph Management
        void write_signals_to_dot_graph();
        void save_dot_graph();
        
    private:
        unsigned long          num_signals_;           // number of signals enumerated in design
        unsigned long          num_signals_ignored_;   // number of signals ignored
        unsigned long          num_local_signals_;     // number of local signals optimized away
        unsigned long          num_constants_;         // number of constants enumerated in design
        unsigned long          num_connections_;       // number of connections enumerated in design
        unsigned long          num_local_connections_; // number of local connections to be processed
        bool                   inside_ff_block_;       // indicates if processing inside a FF block
        bool                   ignore_constants_;      // indicates if constants should be ignored
        bool                   track_source_slices_;   //
        bool                   track_sink_slices_;     //
        string                 clk_basename_;          // indicates if processing inside a FF block
        DotGraph*              dg_;                    // dot graph object
        sig_map_t              signals_map_;           // IVL signal to Signal object map
        signals_q_t            source_signals_;        // source signal queue
        vector<unsigned int>   num_signals_at_depth_;  // back is num source nodes at current depth
        vector<signal_slice_t> source_slices_;         // source signal slice information stack
        vector<signal_slice_t> sink_slices_;           // sink signal slice information stack
        conn_map_t             connections_map_;       // sink signal to connections map
        conn_map_t             local_connections_map_; // local signal connections to be processed
        string_map_t           signals_to_ignore_;     // names of signals to ignore

        // Signal Enumeration
        void find_signals(ivl_scope_t scope);

        // Connection Tracking Setters
        void add_signal_to_ignore(string signal_basename);
        void load_signals_to_ignore(string file_path);

        // Destructor Helpers
        void delete_signals_map();
        void delete_connections_queue(conn_q_t* conn_q);
        void delete_connections_map();

        // Other
        void process_cmd_line_args(cmd_args_map_t* cmd_args);
}; 

#endif
