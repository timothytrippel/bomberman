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
#include <fstream>

// TTB Headers
#include "ttb_typedefs.h"
#include "signal_graph.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------

SignalGraph::SignalGraph():
    num_signals_(0),
    num_local_signals_(0),
    num_constants_(0),
    num_connections_(0),
    num_local_connections_(0),
    signals_map_(),
    dg_(),
    source_signals_(),
    num_signals_at_depth_(),
    source_slices_(),
    sink_slices_(),
    connections_map_(),
    local_connections_map_(),
    signals_to_ignore_() {} 

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
// -------------------------------- Destructors -------------------------------------
// ----------------------------------------------------------------------------------

SignalGraph::~SignalGraph() {
    // 1. Delete Signals in signals_map_
    // Create a signals map iterator
    sig_map_t::iterator sig_it = signals_map_.begin();
 
    // Iterate over the map using iterator till end
    while (sig_it != signals_map_.end()) {   
        // Delete signal
        delete(sig_it->second);

        // Increment the iterator
        sig_it++;
    }

    // 2. Delete Signals in source_signals_ (if any)
    // Create a signals vector iterator
    signals_q_t::iterator ssig_it = source_signals_.begin();
 
    // Iterate over the map using iterator till end
    while (ssig_it != source_signals_.end()) {   
        // Delete signal
        delete(*ssig_it);

        // Increment the iterator
        ssig_it++;
    }

    // 3. Delete Connections in connections_map_
    // Create a signals map iterator
    conn_map_t::iterator conn_it = connections_map_.begin();
 
    // Iterate over the map using iterator till end
    while (conn_it != connections_map_.end()) {   
        // Delete connections queue
        delete(conn_it->second);

        // Increment the iterator
        conn_it++;
    }

    // 4.
    // @TODO: DOUBLE CHECK ALL LOCAL CONNECTIONS HAVE BEEN PROCESSED

    // 5. Close DotGraph file if it is still open
    dg_.save_graph();
}

// ----------------------------------------------------------------------------------
// ------------------------------- Getters ------------------------------------------
// ----------------------------------------------------------------------------------

// ------------------------------ Graph Stats ---------------------------------------

unsigned long SignalGraph::get_num_connections() const {
    return num_connections_;
}

unsigned long SignalGraph::get_num_signals() const {
    // Check that number of signal is correct
    assert((num_signals_ + num_local_signals_) == signals_map_.size() &&
        "ERROR: number of signals does not match size of signals map.\n");

    return num_signals_;
}

unsigned long SignalGraph::get_num_local_signals() const {
    // Check that number of signal is correct
    assert((num_signals_ + num_local_signals_) == signals_map_.size() &&
        "ERROR: number of signals does not match size of signals map.\n");

    return num_local_signals_;
}

unsigned long SignalGraph::get_num_constants() const {
    return num_constants_;
}

// ------------------------------ Signals Map ---------------------------------------

sig_map_t SignalGraph::get_signals_map() const {
    return signals_map_;
}

bool SignalGraph::in_signals_map(Signal* signal) const {
    if (!signal) {
        // Signal is NULL
        return false;
    } else {
        return signals_map_.count(signal->get_ivl_signal());
    }
}

Signal* SignalGraph::get_signal_from_ivl_signal(ivl_signal_t ivl_signal) {
    sig_map_t::iterator sig_it = signals_map_.end();

    // Check if IVL signal exists in map
    if ((sig_it = signals_map_.find(ivl_signal)) != signals_map_.end()) {
        return sig_it->second;
    } else {
        return NULL;
    }
}

// --------------------------- Source Nodes Queue -----------------------------------

unsigned long SignalGraph::get_num_source_signals() const {
    return source_signals_.size();
}

signals_q_t SignalGraph::get_source_signals_queue() const {
    return source_signals_;   
}

Signal* SignalGraph::get_source_signal(unsigned int index) const {
    // Check index is within vector bounds and 
    // vector is NOT empty
    assert(index >= 0 && 
        index < get_num_source_signals() &&
        get_num_source_signals() != 0 &&
        "ERROR: index is outside bounds of source_nodes_ vector.\n");

    return source_signals_[index];
}

Signal* SignalGraph::pop_from_source_signals_queue() {
    Signal* source_signal = NULL;

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

signal_slice_t SignalGraph::pop_from_source_slices_queue(Signal* source_signal) {
    // Source slice information
    signal_slice_t source_slice = {
            source_signal->get_msb(), 
            source_signal->get_lsb()
        };

    if (get_num_source_slices() > 0) {
        source_slice = source_slices_.back();
        source_slices_.pop_back();
    }

    return source_slice;
}

signal_slice_t SignalGraph::pop_from_sink_slices_queue(Signal* sink_signal) {
    // Sink slice information
    signal_slice_t sink_slice = {
            sink_signal->get_msb(),
            sink_signal->get_lsb()
        };

    if (get_num_sink_slices() > 0) {
        sink_slice = sink_slices_.back();
        sink_slices_.pop_back();
    }

    return sink_slice;
}

// --------------------------- Connection Tracking ----------------------------------

bool SignalGraph::check_if_connection_exists(Signal*     sink_signal, 
                                             Connection* new_conn) {

    conn_q_t::iterator conn_it = connections_map_[sink_signal]->begin();

    while (conn_it != connections_map_[sink_signal]->end()) {
        if (**conn_it == *new_conn) {
            return true;
        }

        conn_it++;
    }

    return false;
}

bool SignalGraph::check_if_local_connection_exists(Signal*     sink_signal, 
                                                   Connection* new_conn) {

    if (local_connections_map_.count(sink_signal)) {
        conn_q_t::iterator conn_it = local_connections_map_[sink_signal]->begin();

        while (conn_it != local_connections_map_[sink_signal]->end()) {
            if (**conn_it == *new_conn) {
                return true;
            }

            conn_it++;
        }
    }

    return false;
}

bool SignalGraph::check_if_ignore_signal(Signal* signal) const {
    // Check if ignore map is empty
    if (!signals_to_ignore_.empty()) {

        // Check that signal is not a constant (only ignore signals)
        if (signal->is_signal()) {
            return signals_to_ignore_.count(signal->get_basename());
        }
    }

    return false;
}

bool SignalGraph::check_if_ignore_signal(ivl_signal_t signal) const { 
    // Check if ignore map is empty
    if (!signals_to_ignore_.empty()) {
        return signals_to_ignore_.count(ivl_signal_basename(signal));
    }

    return false;
}

string_map_t SignalGraph::get_signals_to_ignore() const {
    return signals_to_ignore_;
}

// ----------------------------------------------------------------------------------
// ------------------------------- Setters ------------------------------------------
// ----------------------------------------------------------------------------------

// --------------------------- Source Nodes Queue -----------------------------------

void SignalGraph::push_to_source_signals_queue(Signal* source_signal, string ws) {
    // Check that source signal is valid (not NULL)
    assert(source_signal && "ERROR: attempting to push NULL source signal to queue.\n");

    fprintf(stdout, "%sadding source node (%s) to connection queue\n", 
        ws.c_str(), 
        source_signal->get_fullname().c_str());

    source_signals_.push_back(source_signal);
}

// ------------------------- Enumeration Depth Queue --------------------------------

void SignalGraph::push_to_num_signals_at_depth_queue(unsigned int num_signals) {
    num_signals_at_depth_.push_back(num_signals);
}

// ---------------------------- Connection Tracking ---------------------------------

void SignalGraph::track_local_signal_connection(Signal* sink_signal, 
                                                Signal* source_signal,
                                                string  ws) {

    signal_slice_t source_slice;
    signal_slice_t sink_slice;
    Connection*    conn;

    // Check that source signal is valid (not null)
    assert(source_signal && 
        "ERROR: invalid (NULL) pointer to source signal.\n");

    // Check that sink signal is valid (not null)
    assert(sink_signal && 
        "ERROR: invalid (NULL) pointer to sink signal.\n");

    // Check that sink signal is NOT a constant
    assert(!sink_signal->is_const() && 
        "ERROR: sink signal cannot be a constant.\n");

    fprintf(stdout, "%sTracking local signal connection.\n", ws.c_str());

    // Check if connecting signal is already in signals map,
    // i.e. it is an ivl_signal. If it is not in the signals map,
    // check that it is a constant signal (i.e. an ivl_const or
    // ivl_expr). 
    //
    // **NOTE**: Constants are NOT stored in signals map for
    // memory efficiency, but references to them are stored in
    // the connection objects. Hence the Connection destructor
    // calls delete on pointers to constant Signals.
    if (in_signals_map(source_signal) || source_signal->is_const()) {

        // ONLY process connections between ivl-generated source signals
        if (source_signal->is_ivl_generated() || sink_signal->is_ivl_generated()) {

            // Get signal slices
            source_slice = pop_from_source_slices_queue(source_signal);
            sink_slice   = pop_from_sink_slices_queue(sink_signal);

            // Create connection object
            conn = new Connection(source_signal, sink_signal, source_slice, sink_slice);

            // Check if local connection already exists
            if (!check_if_local_connection_exists(sink_signal, conn)) {
                
                // Create connections list if it does not exist
                if (!local_connections_map_.count(sink_signal)) {
                    local_connections_map_[sink_signal] = new conn_q_t();
                }

                // Save Connection
                local_connections_map_[sink_signal]->push_back(conn);

                // Increment connection counter
                num_local_connections_++;  

            } else {

                fprintf(stdout, "%slocal connection (from %s to %s) already exists...\n", 
                    ws.c_str(),
                    conn->get_source()->get_fullname().c_str(),
                    conn->get_sink()->get_fullname().c_str());

            }
        } else {
            Error::non_local_signal_connection();
        }

    } else {

        Error::connecting_signal_not_in_graph(
            signals_map_, source_signal->get_ivl_signal());

    }
}

void SignalGraph::add_signal_to_ignore(string signal_basename) {
    signals_to_ignore_[signal_basename] = true;
}

void SignalGraph::load_signals_to_ignore(string file_path) {
    // Ignore file stream object
    ifstream file_stream = ifstream(file_path);

    // Signal basename (line) read from file stream
    string signal_basename;

    // Open file
    if (file_stream.is_open()) {

        // Process File
        while (getline(file_stream, signal_basename)) {
            signals_to_ignore_[signal_basename] = true;
        }

        // Close File
        file_stream.close();

    } else {
        fprintf(stderr, "ERROR: Could not open file %s\n", file_path.c_str());
        exit(FILE_ERROR);
    }
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

        // Check if signal is to be ignored
        if (!check_if_ignore_signal(current_signal)) {
            
            // Add signal to map
            signals_map_[current_signal] = new Signal(current_signal);

            // Add signal to graph
            // Ignore local (IVL) generated signals.
            if (!ivl_signal_local(current_signal)) {
                // Intialize connections queue for signal
                connections_map_[signals_map_[current_signal]] = new conn_q_t();

                // Add signal to dot graph
                dg_.add_node(signals_map_[current_signal], WS_TAB);

                // Increment Signals Counter
                num_signals_++;
            } else {

                // Increment Local Signals Counter
                num_local_signals_++;
            }
        }
    } 
}

// ----------------------------------------------------------------------------------
// ------------------------------- Connection Enumeration ---------------------------
// ----------------------------------------------------------------------------------

void SignalGraph::add_connection(Signal* sink_signal, 
                                 Signal* source_signal, 
                                 string  ws) {

    signal_slice_t source_slice;
    signal_slice_t sink_slice;
    Connection*    conn;

    // Check that source signal is valid (not null)
    assert(source_signal && 
        "ERROR: invalid (NULL) pointer to source signal.\n");

    // Check that sink signal is valid (not null)
    assert(sink_signal && 
        "ERROR: invalid (NULL) pointer to sink signal.\n");

    // Check that sink signal is NOT a constant
    assert(!sink_signal->is_const() && 
        "ERROR: sink signal cannot be a constant.\n");

    // Check if connecting signal is already in signals map,
    // i.e. it is an ivl_signal. If it is not in the signals map,
    // check that it is a constant signal (i.e. an ivl_const or
    // ivl_expr). 
    //
    // **NOTE**: Constants are NOT stored in signals map for
    // memory efficiency, but references to them are stored in
    // the connection objects. Hence the Connection destructor
    // calls delete on pointers to constant Signals.
    if (in_signals_map(source_signal) || source_signal->is_const()) {

        // Do not process connections to ivl-generated source signals
        if (!source_signal->is_ivl_generated()) {

            // Get signal slices
            source_slice = pop_from_source_slices_queue(source_signal);
            sink_slice   = pop_from_sink_slices_queue(sink_signal);

            // Create connection object
            conn = new Connection(source_signal, sink_signal, source_slice, sink_slice);

            // Check if connection already exists
            if (!check_if_connection_exists(sink_signal, conn)) {
                
                // Add node to graph if it is a CONSTANT
                if (source_signal->is_const()) {
                    dg_.add_node(source_signal, ws);
                    num_constants_++;
                }

                // Add connection to dot graph
                dg_.add_connection(conn, ws);
                
                // Save Connection
                connections_map_[sink_signal]->push_back(conn);

                // Increment connection counter
                num_connections_++;  

            } else {
                fprintf(stdout, "%sconnection (from %s to %s) already exists...\n", 
                    ws.c_str(),
                    conn->get_source()->get_fullname().c_str(),
                    conn->get_sink()->get_fullname().c_str());
            }
        }

    } else {

        Error::connecting_signal_not_in_graph(
            signals_map_, source_signal->get_ivl_signal());

    }
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

void SignalGraph::process_local_connections(string ws) {
    Signal*     sink_signal  = NULL;
    Signal*     local_signal = NULL;
    Connection* current_conn = NULL;

    conn_map_t::iterator conn_map_it = local_connections_map_.begin();

    while (conn_map_it != local_connections_map_.end()) {

        // Start at sink signals
        if (!conn_map_it->first->is_ivl_generated()) {
            sink_signal = conn_map_it->first;

            // Check that only one connection to sink signal exists
            assert(local_connections_map_[sink_signal]->size() == 1 &&
                "ERROR: invalid local connection to sink signal.\n");

            // Get local (middle-man) signal
            local_signal = local_connections_map_[sink_signal]->back()->get_source();

            // Check that the (one) connection to sink signal is local
            assert(local_signal->is_ivl_generated() &&
                "ERROR: signal connected to sink signal is NOT local.\n");

            // Delete sink connections queue
            local_connections_map_[sink_signal]->pop_back();
            delete(local_connections_map_[sink_signal]);
            // local_connections_map_.erase(sink_signal);

            // Iterate over source signals connected to local signal
            while (!local_connections_map_[local_signal]->empty()) {
                
                // Get current connection obj
                current_conn = local_connections_map_[local_signal]->back();

                // Modify sink signal of connection obj
                current_conn->set_sink(sink_signal);

                // Add connection to dot graph
                dg_.add_connection(current_conn, ws);
                num_connections_++;

                // Remove connection from queue after it is processed
                local_connections_map_[local_signal]->pop_back();

                // Delete connection obj
                delete(current_conn);
            }

            // Delete connections queue
            delete(local_connections_map_[local_signal]);
            // local_connections_map_.erase(local_signal);

        }

        conn_map_it++;
    }
}
