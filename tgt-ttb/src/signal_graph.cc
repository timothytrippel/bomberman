/*
File:        signal_graph.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
// #include <cassert>
// #include <cstdio>
// #include <sstream>

// TTB Headers
#include "signal_graph.h"
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------
SignalGraph::SignalGraph():
    num_connections_(0),
    signals_map_(),
    dg_(),
    signal_slices_(),
    source_signals_() {} 

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

unsigned long SignalGraph::get_num_source_signals() {
    return source_signals_.size();
}

sig_map_t SignalGraph::get_signals_map() {
    return signals_map_;
}

sig_q_t SignalGraph::get_source_signals_queue() {
	return source_signals_;	
}

ivl_signal_t SignalGraph::pop_from_source_signals_queue() {
	// Check if source signals queue is not empty
	if (get_num_source_signals() > 0) {
		// Get last signal in queue
		ivl_signal_t source_signal = source_signals_.back();

		// Remove last signal in queue
		source_signals_.pop_back();

		// Return removed signal
		return source_signal;
	} else {
		return NULL;
	}
}

// ----------------------------------------------------------------------------------
// ------------------------------- Setters ------------------------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::push_to_source_signals_queue(ivl_signal_t source_signal, string ws) {
	fprintf(stdout, "%sadding source signal (%s) to queue\n", 
        ws.c_str(), get_signal_fullname(source_signal).c_str());

	source_signals_.push_back(source_signal);
}

// ----------------------------------------------------------------------------------
// ------------------------------- Dot Graph Management -----------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::save_dot_graph() {
    dg_.save_graph();
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

        // Check if signal is arrayed
        // @TODO: support arrayed signals 
        // (i.e. signals with more than one nexus)
        Error::check_signal_not_arrayed(current_signal);

        // Check if signal is multi-dimensional
        // @TODO: support multi-dimensional signals 
        // (i.e. signals packed dimensions like memories)
        Error::check_signal_not_multidimensional(current_signal);

        // Check if signal already exists in map
        Error::check_signal_exists_in_map(signals_map_, current_signal);

        // Add signal to graph
        // Ignore local (IVL) generated signals.
        if (!ivl_signal_local(current_signal)) {
            // signal was defined in HDL
            signals_map_[current_signal] = vector<ivl_signal_t>();
            dg_.add_node(get_signal_fullname(current_signal), 
                         get_signal_node_label(current_signal), 
                         SIGNAL_NODE_SHAPE);
        }
    } 
}

// ----------------------------------------------------------------------------------
// ------------------------------- Connection Enumeration ---------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::add_constant_connection(ivl_signal_t    sink_signal, 
                                          ivl_net_const_t source_constant,
                                          string          ws) {

    string source_constant_name;
    string source_constant_label;
    string sink_signal_name;
    string connection_label;

    // Get constant node name and label
    source_constant_name  = get_constant_fullname(source_constant, num_constants_);
    source_constant_label = get_const_node_label(source_constant);
    
    // Get sink signal names
    sink_signal_name = get_signal_fullname(sink_signal);

    // Get connection label
    connection_label = get_const_connection_label(source_constant, sink_signal);

    // Debug Print
    fprintf(stdout, "%sADDING CONSTANT NODE (%s)\n", 
        ws.c_str(), 
        source_constant_name.c_str());

    // Add CONSTANT node to dot graph
    dg_.add_node(source_constant_name, source_constant_label, CONST_NODE_SHAPE);

    // Increment constants counter
    num_constants_++;

    // Add connection to dot graph
    add_connection(source_constant_name, sink_signal_name, connection_label, ws);
}

void SignalGraph::add_constant_expr_connection(ivl_signal_t sink_signal, 
                                               ivl_expr_t   source_const_expr,
                                               string       ws) {
    
    string source_constant_name;
    string source_constant_label;
    string sink_signal_name;
    string connection_label;

    // Get constant node name and label
    source_constant_name  = get_constant_expr_fullname(source_const_expr, num_constants_);
    source_constant_label = get_const_expr_node_label(source_const_expr);
    
    // Get sink signal names
    sink_signal_name = get_signal_fullname(sink_signal);

    // Get connection label
    connection_label = get_const_expr_connection_label(source_const_expr, sink_signal);

    // Debug Print
    fprintf(stdout, "%sADDING CONSTANT EXPRESSION NODE (%s)\n", 
        ws.c_str(), 
        source_constant_name.c_str());

    // Add CONSTANT node to dot graph
    dg_.add_node(source_constant_name, source_constant_label, CONST_NODE_SHAPE);

    // Increment constants counter
    num_constants_++;

    // Add connection to dot graph
    add_connection(source_constant_name, sink_signal_name, connection_label, ws);
}

void SignalGraph::add_signal_connection(ivl_signal_t sink_signal, 
                                        ivl_signal_t source_signal, 
                                        string       ws) {

    string source_signal_name;
    string sink_signal_name;
    string connection_label;

    // Only add connections to signals already in graph.
    // Thus, ignoring local IVL generated signals as these
    // are not added to the graph when it is initialized.
    if (signals_map_.count(source_signal)) {
    	// Add signal object to adjacency list
        signals_map_[sink_signal].push_back(source_signal);
        
        // Get signal names
        source_signal_name = get_signal_fullname(source_signal);
        sink_signal_name   = get_signal_fullname(sink_signal);

        // Check if connection is sliced
        if (signal_slices_.size()) {
            // SLICED connection

            // Get signal slice info
            SliceInfo signal_slice = signal_slices_.back();

            // Get connection label
            connection_label = get_sliced_signal_connection_label(source_signal, 
                                                                  sink_signal, 
                                                                  signal_slice);

            // pop slice info from stack
            signal_slices_.pop_back();
        } else {
            // FULL connection

            // Get connection label
            connection_label = get_signal_connection_label(source_signal, sink_signal);
        }

        // Add connection to dot graph
        add_connection(source_signal_name, sink_signal_name, connection_label, ws);
    } else if (!ivl_signal_local(source_signal)) {
        Error::connecting_signal_not_in_graph(source_signal);
    }
}

void SignalGraph::add_connection(string source_signal_name, 
                                 string sink_signal_name, 
                                 string connection_label,
                                 string ws) {

	// Debug Print
    fprintf(stdout, "%sADDING CONNECTION from %s to %s\n", 
        ws.c_str(), 
        source_signal_name.c_str(), 
        sink_signal_name.c_str());

	// Add connection to dot graph
    dg_.add_connection(source_signal_name, sink_signal_name, connection_label);
    num_connections_++;
}

void SignalGraph::track_lpm_connection_slice(unsigned int      msb, 
                                             unsigned int      lsb,
                                             slice_node_type_t node_type){

    // Set signal slice info
    SliceInfo slice_info = {msb, lsb, node_type};

    // Check that slice-info stack is empty
    // (it should never grow beyond a size of 1)
    assert(signal_slices_.size() <= 1 && 
        "ERROR: slice info stack NOT empty.\n");

    // push slice info to a stack
    signal_slices_.push_back(slice_info);
}
