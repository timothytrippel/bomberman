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

// TTB Headers
#include "ttb_typedefs.h"
#include "signal_graph.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------
SignalGraph::SignalGraph():
    num_constants_(0),
    num_connections_(0),
    signals_map_(),
    dg_(),
    source_signals_(),
    num_signals_at_depth_(),
    source_slices_(),
    sink_slices_() {} 

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

// ------------------------------ Graph Stats ---------------------------------------
unsigned long SignalGraph::get_num_connections() const {
    return num_connections_;
}

unsigned long SignalGraph::get_num_signals() const {
    return signals_map_.size();
}

unsigned long SignalGraph::get_num_constants() const {
    return num_constants_;
}

// ------------------------------ Signals Map ---------------------------------------
sig_map_t SignalGraph::get_signals_map() const {
    return signals_map_;
}

// --------------------------- Source Nodes Queue -----------------------------------
unsigned long SignalGraph::get_num_source_signals() const {
    return source_signals_.size();
}

vector<Signal> SignalGraph::get_source_signals_queue() const {
    return source_signals_;   
}

Signal SignalGraph::get_source_signal(unsigned int index) const {
    // Check index is within vector bounds and 
    // vector is NOT empty
    assert(index >= 0 && 
        index < get_num_source_signals() &&
        get_num_source_signals() != 0 &&
        "ERROR: index is outside bounds of source_nodes_ vector.\n");

    return source_signals_[index];
}

Signal SignalGraph::pop_from_source_signals_queue() {
    Signal source_signal;

    // Check if source nodes queue is not empty
    if (get_num_source_signals() > 0) {
        // Get last signal in queue
        source_signal = source_signals_.back();

        // Remove last signal in queue
        source_signals_.pop_back();   
    }

    // Return removed node
    return source_signal;
}

void SignalGraph::pop_from_source_signals_queue(unsigned int num_signals) {
    for (unsigned int i = 0; i < num_signals; i++) {
        // Check if source signals queue is not empty
        if (get_num_source_signals() > 0) {
            // Remove last signal in queue
            source_signals_.pop_back();   
        } else {
            break;
        }
    }
}

// ------------------------- Enumeration Depth Queue --------------------------------
unsigned long SignalGraph::get_scope_depth() const {
    return num_signals_at_depth_.size();
}

unsigned int SignalGraph::pop_from_num_signals_at_depth_queue() {
    unsigned int num_signals = 0;

    // Check if source nodes queue is not empty
    if (get_scope_depth() > 0) {
        // Get last node in queue
        num_signals = num_signals_at_depth_.back();

        // Remove last node in queue
        num_signals_at_depth_.pop_back();   
    }

    // Return removed node
    return num_signals;
}

// -------------------------- Slice Tracking Queues ---------------------------------
unsigned int SignalGraph::get_num_source_slices() const {
    return source_slices_.size();
}

unsigned int SignalGraph::get_num_sink_slices() const {
    return sink_slices_.size();
}

signal_slice_t SignalGraph::pop_from_source_slices_queue(Signal source_signal) {
    // Source slice information
    signal_slice_t source_slice = {
            source_signal.get_msb(), 
            source_signal.get_lsb()
        };

    if (get_num_source_slices() > 0) {
        source_slice = source_slices_.back();
        source_slices_.pop_back();
    }

    return source_slice;
}

signal_slice_t SignalGraph::pop_from_sink_slices_queue(Signal sink_signal) {
    // Sink slice information
    signal_slice_t sink_slice = {
            sink_signal.get_msb(),
            sink_signal.get_lsb()
        };

    if (get_num_sink_slices() > 0) {
        sink_slice = sink_slices_.back();
        sink_slices_.pop_back();
    }

    return sink_slice;
}

// ----------------------------------------------------------------------------------
// ------------------------------- Setters ------------------------------------------
// ----------------------------------------------------------------------------------
// --------------------------- Source Nodes Queue -----------------------------------
void SignalGraph::push_to_source_signals_queue(Signal source_signal, string ws) {
    fprintf(stdout, "%sadding source node (%s) to connection queue\n", 
        ws.c_str(), 
        source_signal.get_fullname().c_str());

    source_signals_.push_back(source_signal);
}

// ------------------------- Enumeration Depth Queue --------------------------------
void SignalGraph::push_to_num_signals_at_depth_queue(unsigned int num_signals) {
    num_signals_at_depth_.push_back(num_signals);
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
    for (unsigned int i = 0; i < num_scopes; i++) {
        find_signals(scopes[i]);
    }
}

void SignalGraph::find_signals(ivl_scope_t scope) {
    // Current signal
    ivl_signal_t current_signal;

    // Recurse into sub-modules
    for (unsigned int i = 0; i < ivl_scope_childs(scope); i++) {
        find_signals(ivl_scope_child(scope, i));        
    }

    // Scope is a base scope
    unsigned int num_signals = ivl_scope_sigs(scope);
    for (unsigned int i = 0; i < num_signals; i++) {
        // Get current signal
        current_signal = ivl_scope_sig(scope, i);

        // Check if signal is arrayed
        // @TODO: support arrayed signals 
        // (i.e. signals with more than one nexus)
        Error::check_signal_not_arrayed(signals_map_, current_signal);

        // Check if signal is multi-dimensional
        // @TODO: support multi-dimensional signals 
        // (i.e. signals packed dimensions like memories)
        Error::check_signal_not_multidimensional(signals_map_, current_signal);

        // Check if signal already exists in map
        Error::check_signal_exists_in_map(signals_map_, current_signal);

        // Add signal to graph
        // Ignore local (IVL) generated signals.
        if (!ivl_signal_local(current_signal)) {
            // signal was defined in HDL
            signals_map_[current_signal] = Signal(current_signal);
            dg_.add_node(signals_map_[current_signal], WS_TAB);
        }
    } 
}

// ----------------------------------------------------------------------------------
// ------------------------------- Connection Enumeration ---------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::add_constant_connection(Signal sink_signal, 
                                          Signal source_signal,
                                          string ws) {

    string         source_signal_name;
    string         sink_signal_name;
    signal_slice_t source_slice;
    signal_slice_t sink_slice;
    Connection     conn;

    // Get source signal names
    source_signal_name = source_signal.get_fullname() + 
                            string(".") + 
                            to_string(num_constants_);

    // Get signal slices
    source_slice = pop_from_source_slices_queue(source_signal);
    sink_slice   = pop_from_sink_slices_queue(sink_signal);

    // Create connection object
    conn = Connection(source_signal, sink_signal, source_slice, sink_slice);

    // Add CONSTANT node to dot graph
    dg_.add_node(source_signal, ws);

    // Increment constants counter
    num_constants_++;

    // Add connection to dot graph
    dg_.add_connection(conn, ws);
}

void SignalGraph::add_signal_connection(Signal sink_signal, 
                                        Signal source_signal, 
                                        string ws) {

    ivl_signal_t   source_ivl_signal;
    ivl_signal_t   sink_ivl_signal;
    string         source_signal_name;
    string         sink_signal_name;
    signal_slice_t source_slice;
    signal_slice_t sink_slice;
    Connection     conn;

    // Get IVL Signals
    source_ivl_signal = source_signal.get_ivl_signal();
    sink_ivl_signal   = source_signal.get_ivl_signal();

    // Only add connections to signals already in graph.
    // Thus, ignoring local IVL generated signals as these
    // are not added to the graph when it is initialized.
    if (signals_map_.count(source_ivl_signal)) {

        // Get signal slices
        source_slice = pop_from_source_slices_queue(source_signal);
        sink_slice   = pop_from_sink_slices_queue(sink_signal);

        // Create connection object
        conn = Connection(source_signal, sink_signal, source_slice, sink_slice);

        // Add connection to dot graph
        dg_.add_connection(conn, ws);

    } else if (!source_signal.is_ivl_generated()) {

        Error::connecting_signal_not_in_graph(signals_map_, source_ivl_signal);

    }
}

void SignalGraph::add_connection(Signal sink_signal,
                                 Signal source_signal,
                                 string ws) {

    // Add connection
    switch(source_signal.get_ivl_type()) {
        case IVL_NONE:
            Error::null_ivl_obj_type();
            break;
        case IVL_SIGNAL:
            add_signal_connection(sink_signal, source_signal, ws);
            break;
        case IVL_CONST:
        case IVL_EXPR:
            add_constant_connection(sink_signal, source_signal, ws);
            break;
        default:
            Error::unknown_ivl_obj_type(source_signal.get_ivl_type());
            break;
    }

    // Increment connection counter
    num_connections_++;
}

void SignalGraph::track_source_slice(unsigned int msb, 
                                     unsigned int lsb,
                                     string       ws) {

    // Check that slice-info stacks are empty,
    // (Stacks should never grow beyond size 1.)
    Error::check_slice_tracking_stack(source_slices_);

    // Set signal slice info
    signal_slice_t slice = {msb, lsb};

    // Debug Print
    fprintf(stdout, "%sTracking signal slice [%u:%u]\n", 
        ws.c_str(), msb, lsb);

    // push slice info to stack
    source_slices_.push_back(slice);
}

void SignalGraph::track_sink_slice(unsigned int msb, 
                                   unsigned int lsb,
                                   string       ws) {

    // Check that slice-info stacks are empty,
    // (Stacks should never grow beyond size 1.)
    Error::check_slice_tracking_stack(sink_slices_);

    // Set signal slice info
    signal_slice_t slice = {msb, lsb};

    // Debug Print
    fprintf(stdout, "%sTracking signal slice [%u:%u]\n", 
        ws.c_str(), msb, lsb);

    // push slice info to stack
    sink_slices_.push_back(slice);
}
