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
#include <sstream>

// TTB Headers
#include "ttb.h"
#include "reporter.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------
SignalGraph::SignalGraph():
    num_connections_(0),
    signals_map_(),
    dg_(),
    signal_slices_() {}

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

sig_map_t SignalGraph::get_signals_map() {
    return signals_map_;
}

// ----------------------------------------------------------------------------------
// --------------------------- Static Helper Functions ------------------------------
// ----------------------------------------------------------------------------------
string SignalGraph::get_signal_fullname(ivl_signal_t signal){
    string scopename = ivl_scope_name(ivl_signal_scope(signal)); 
    string basename  = ivl_signal_basename(signal);
    string fullname  = scopename + string(".") + basename;

    return fullname;
}

string SignalGraph::get_constant_fullname(ivl_net_const_t constant){
    string scopename = ivl_scope_name(ivl_const_scope(constant)); 
    string basename  = string(ivl_const_bits(constant), (size_t)ivl_const_width(constant));
    reverse(basename.begin(), basename.end());
    string fullname  = scopename + string(".const_") + basename;

    return fullname;
}

unsigned int SignalGraph::get_signal_msb(ivl_signal_t signal) {
    if (ivl_signal_packed_dimensions(signal) > 0) {
        // Check MSB is not negative
        assert((ivl_signal_packed_msb(signal, 0) >= 0) && \
            "NOT-SUPPORTED: negative MSB index.\n");
        
        return ivl_signal_packed_msb(signal, 0);
    } else {
        return 0;
    }
}

unsigned int SignalGraph::get_signal_lsb(ivl_signal_t signal) {
    if (ivl_signal_packed_dimensions(signal) > 0) {
        // Check LSB is not negative
        assert((ivl_signal_packed_lsb(signal, 0) >= 0) && \
            "NOT-SUPPORTED: negative LSB index.\n");

        return ivl_signal_packed_lsb(signal, 0);
    } else {
        return 0;
    }
}

unsigned int SignalGraph::get_const_msb(ivl_net_const_t constant) {
    return ivl_const_width(constant) - 1;
}

string SignalGraph::get_signal_node_label(ivl_signal_t signal) {
    stringstream ss;

    ss << "[";
    ss << get_signal_msb(signal);
    ss << ":";
    ss << get_signal_lsb(signal);
    ss << "]";

    return ss.str();
}

string SignalGraph::get_const_node_label(ivl_net_const_t constant) {
    stringstream ss;

    ss << "[";
    ss << get_const_msb(constant);
    ss << ":0]";

    return ss.str();
}

string SignalGraph::get_signal_connection_label(ivl_signal_t source_signal, 
                                                ivl_signal_t sink_signal) {
    stringstream ss;
    
    ss << get_signal_node_label(source_signal);
    ss << "->";
    ss << get_signal_node_label(sink_signal);

    return ss.str();
}

string SignalGraph::get_const_connection_label(ivl_net_const_t source_constant, 
                                               ivl_signal_t    sink_signal) {

    stringstream ss;
    
    ss << get_const_node_label(source_constant);
    ss << "->";
    ss << get_signal_node_label(sink_signal);

    return ss.str();
}

string SignalGraph::get_sliced_signal_connection_label(ivl_signal_t source_signal, 
                                                       ivl_signal_t sink_signal, 
                                                       SliceInfo    signal_slice) {
    unsigned int sink_msb;
    unsigned int sink_lsb;
    unsigned int source_msb;
    unsigned int source_lsb;
    stringstream ss;

    if (signal_slice.node == SINK) {
        // sliced sink node (signal)
        sink_msb = signal_slice.msb;
        sink_lsb = signal_slice.lsb;
        source_msb = get_signal_msb(source_signal);
        source_lsb = get_signal_lsb(source_signal);
    } else {
        // sliced source node (signal)
        sink_msb = get_signal_msb(sink_signal);
        sink_lsb = get_signal_lsb(sink_signal);
        source_msb = signal_slice.msb;
        source_lsb = signal_slice.lsb;
    }
    
    
    ss << "[";
    ss << source_msb;
    ss << ":";
    ss << source_lsb;
    ss << "]->[";
    ss << sink_msb;
    ss << ":";
    ss << sink_lsb;
    ss << "]";

    return ss.str();
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
    source_constant_name  = get_constant_fullname(source_constant);
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

    // Debug Print
    fprintf(stdout, "%sADDING CONNECTION from %s to %s\n", 
        ws.c_str(), 
        source_constant_name.c_str(), 
        sink_signal_name.c_str());

    // Add connection to dot graph
    dg_.add_connection(source_constant_name, sink_signal_name, connection_label);
}

void SignalGraph::add_connection(ivl_signal_t sink_signal, 
                                 ivl_signal_t source_signal, 
                                 string       ws) {

    string source_signal_name;
    string sink_signal_name;
    string connection_label;

    // Only add connections to signals already in graph.
    // Thus, ignoring local IVL generated signals as these
    // are not added to the graph when it is initialized.
    if (signals_map_.count(source_signal)) {

        signals_map_[sink_signal].push_back(source_signal);

        // Debug Print
        fprintf(stdout, "%sADDING CONNECTION from %s to %s\n", 
            ws.c_str(), 
            get_signal_fullname(source_signal).c_str(), 
            get_signal_fullname(sink_signal).c_str());
        
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
        dg_.add_connection(source_signal_name, sink_signal_name, connection_label);
    } else if (!ivl_signal_local(source_signal)) {
        Error::connecting_signal_not_in_graph(source_signal);
    }
}

void SignalGraph::find_all_connections() {
    // Create a signals map iterator
    sig_map_t::iterator it = signals_map_.begin();
 
    // Iterate over all signals in adjacency list
    while (it != signals_map_.end()) {  
        ivl_signal_t sink_signal = it->first;

        // Print signal name -- signal dimensions
        fprintf(stdout, "%s:\n", get_signal_fullname(sink_signal).c_str());

        // Get signal nexus
        // There is exactly one nexus for each WORD of a signal.
        // Since we only support non-arrayed signals (above), 
        // each signal only has one nexus.
        const ivl_nexus_t sink_nexus = ivl_signal_nex(sink_signal, 0);

        // Check Nexus IS NOT NULL
        assert(sink_nexus);

        // Propagate the nexus
        propagate_nexus(sink_nexus, sink_signal, "  ");

        // Increment the iterator
        it++;
    }
}

// ----------------------------------------------------------------------------------
// ------------------------ IVL Target Entry Point "main" ---------------------------
// ----------------------------------------------------------------------------------
int target_design(ivl_design_t des) {
    ivl_scope_t*   roots     = 0;       // root scopes of the design
    unsigned       num_roots = 0;       // number of root scopes of the design
    Reporter       reporter;            // reporter object (prints messages)
    SignalGraph    sg;                  // signal graph object

    // Initialize reporter checking objects
    reporter = Reporter();
    reporter.init(LAUNCH_MESSAGE);

    // Initialize SignalGraph
    sg = SignalGraph(ivl_design_flag(des, "-o"));
    
    // Get root scopes (top level modules) of design
    reporter.print_message(SCOPE_EXPANSION_MESSAGE);
    ivl_design_roots(des, &roots, &num_roots);
    Error::check_scope_types(roots, num_roots);
    reporter.root_scopes(roots, num_roots);

    // Find all critical signals and dependencies in the design
    reporter.print_message(SIGNAL_ENUM_MESSAGE);
    sg.find_all_signals(roots, num_roots);
    reporter.num_signals(sg.get_num_signals());
    reporter.signal_names(sg.get_signals_map());

    // Find signal-to-signal connections
    reporter.print_message(CONNECTION_ENUM_MESSAGE);
    sg.find_all_connections();
    reporter.num_connections(sg.get_num_connections());

    // Save dot graph to file
    sg.save_dot_graph();

    // Report total execution time
    reporter.end();

    return 0;
}
