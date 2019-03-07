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
    num_constants_(0),
    num_connections_(0),
    signals_map_(),
    dg_(),
    node_slices_(),
    source_nodes_() {} 

SignalGraph::SignalGraph(const char* dot_graph_fname) {
    // Initialize Constants Counter
    num_constants_ = 0;

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

unsigned long SignalGraph::get_num_constants() {
    return num_constants_;
}

unsigned long SignalGraph::get_num_source_nodes() {
    return source_nodes_.size();
}

sig_map_t SignalGraph::get_signals_map() {
    return signals_map_;
}

node_q_t SignalGraph::get_source_nodes_queue() {
	return source_nodes_;	
}

node_t SignalGraph::pop_from_source_nodes_queue() {
	node_t source_node = {{NULL}, IVL_NONE};

	// Check if source nodes queue is not empty
	if (get_num_source_nodes() > 0) {
		// Get last node in queue
		source_node = source_nodes_.back();

		// Remove last node in queue
		source_nodes_.pop_back();	
	}

	// Return removed node
	return source_node;
}

string SignalGraph::get_node_fullname(node_t node) {
    switch(node.type) {
        case IVL_NONE:
            Error::null_node_type();
            break;
        case IVL_SIGNAL:
            return get_signal_fullname(node.object.ivl_signal);
        case IVL_CONST:
            return get_constant_fullname(node.object.ivl_constant, num_constants_);
        case IVL_CONST_EXPR:
            return get_constant_expr_fullname(node.object.ivl_const_expr, num_constants_);
        default:
            Error::unknown_node_type(node.type);
            break;
    }
}

// ----------------------------------------------------------------------------------
// ------------------------------- Setters ------------------------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::push_to_source_nodes_queue(node_t source_node, string ws) {
	fprintf(stdout, "%sadding source node (%s) to connection queue\n", 
        ws.c_str(), 
        get_node_fullname(source_node).c_str());

	source_nodes_.push_back(source_node);
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
void SignalGraph::add_constant_connection(ivl_signal_t sink_signal, 
                                          node_t       source_node,
                                          string       ws) {

    string source_constant_name;
    string source_constant_label;
    string sink_signal_name;
    string connection_label;

    // Get constant node name and label
	switch(source_node.type) {

        case IVL_CONST:
            // Get constant name
            source_constant_name = get_constant_fullname(
            	source_node.object.ivl_constant, 
            	num_constants_);

            // Get constant bits label
    		source_constant_label = get_const_node_label(
    			source_node.object.ivl_constant);

    		// Get connnection label
    		connection_label = get_const_connection_label(
    			source_node.object.ivl_constant, 
    			sink_signal);

    		break;

        case IVL_CONST_EXPR:
        	// Get constant name
            source_constant_name = get_constant_expr_fullname(
            	source_node.object.ivl_const_expr, 
            	num_constants_);
    		
    		// Get constant bits label
    		source_constant_label = get_const_expr_node_label(
    			source_node.object.ivl_const_expr);
    		
    		// Get connnection label
    		connection_label = get_const_expr_connection_label(
    			source_node.object.ivl_const_expr,
    			sink_signal);

    		break;

        default:
            Error::not_supported("node type for constant.");
    }

    // Get sink signal names
    sink_signal_name = get_signal_fullname(sink_signal);

    // Debug Print
    fprintf(stdout, "%sADDING CONSTANT NODE (%s)\n", 
        ws.c_str(), 
        source_constant_name.c_str());

    // Add CONSTANT node to dot graph
    dg_.add_node(source_constant_name, source_constant_label, CONST_NODE_SHAPE);

    // Increment constants counter
    num_constants_++;

    // Add connection to dot graph
    dg_.add_connection(source_constant_name, sink_signal_name, connection_label, ws);
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
        if (node_slices_.size()) {
            // SLICED connection

            // Get signal slice info
            node_slice_t signal_slice = node_slices_.back();

            // Get connection label
            connection_label = get_sliced_signal_connection_label(source_signal, 
                                                                  sink_signal, 
                                                                  signal_slice);

            // pop slice info from stack
            node_slices_.pop_back();
        } else {
            // FULL connection

            // Get connection label
            connection_label = get_signal_connection_label(source_signal, sink_signal);
        }

        // Add connection to dot graph
        dg_.add_connection(source_signal_name, sink_signal_name, connection_label, ws);
    } else if (!ivl_signal_local(source_signal)) {
        Error::connecting_signal_not_in_graph(source_signal);
    }
}

void SignalGraph::add_connection(ivl_signal_t sink_signal,
								 node_t       source_node,
                                 string       ws) {

	// Add connection
	switch(source_node.type) {
        case IVL_NONE:
            Error::null_node_type();
            break;
        case IVL_SIGNAL:
            add_signal_connection(sink_signal, source_node.object.ivl_signal, ws);
            break;
        case IVL_CONST:
        case IVL_CONST_EXPR:
            add_constant_connection(sink_signal, source_node, ws);
            break;
        default:
            Error::unknown_node_type(source_node.type);
            break;
    }

    // Increment connection counter
    num_connections_++;
}

void SignalGraph::track_lpm_connection_slice(unsigned int      msb, 
                                             unsigned int      lsb,
                                             node_slice_type_t node_type){

    // Set signal slice info
    node_slice_t slice = {msb, lsb, node_type};

    // Check that slice-info stack is empty
    // (it should never grow beyond a size of 1)
    assert(node_slices_.size() <= 1 && 
        "ERROR: slice info stack NOT empty.\n");

    // push slice info to a stack
    node_slices_.push_back(slice);
}
