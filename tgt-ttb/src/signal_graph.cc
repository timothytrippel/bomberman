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

// ------------------------------------------------------------
// ------------------------ Constructors ----------------------
// ------------------------------------------------------------

SignalGraph::SignalGraph():
    num_signals_(0),
    num_signals_ignored_(0),
    num_local_signals_(0),
    num_constants_(0),
    num_connections_(0),
    num_local_connections_(0),
    dg_(NULL),
    signals_map_(),
    connections_map_(),
    local_connections_map_(),
    signals_to_ignore_() {} 

SignalGraph::SignalGraph(cmd_args_map_t* cmd_args) {

    // Intialize Counters
    num_signals_           = 0;
    num_signals_ignored_   = 0;
    num_local_signals_     = 0;
    num_constants_         = 0;
    num_connections_       = 0;
    num_local_connections_ = 0;

    // Initialize DotGraph
    dg_ = new DotGraph(cmd_args->at(OUTPUT_FILENAME_FLAG));
    dg_->init_graph();

    // Process command line args
    process_cmd_line_args(cmd_args);
}

// ------------------------------------------------------------
// ------------------------ Destructors -----------------------
// ------------------------------------------------------------

SignalGraph::~SignalGraph() {

    fprintf(DESTRUCTOR_PRINTS_FILE_PTR, 
        "Executing SignalGraph destructor...\n");

    // 1. Delete Connections in connections_map_
    fprintf(DESTRUCTOR_PRINTS_FILE_PTR, 
        "Destroying connections map...\n");
    delete_connections_map();
    assert(!connections_map_.size() && 
        "ERROR: some connections remain un-deleted.\n");


    // 2. Delete Signals in signals_map_
    fprintf(DESTRUCTOR_PRINTS_FILE_PTR, 
        "   Destroying signals map...\n");
    delete_signals_map();
    assert(!signals_map_.size() && 
        "ERROR: some signals remain un-deleted.\n");

    // 3. Close DotGraph file if it is still open
    fprintf(DESTRUCTOR_PRINTS_FILE_PTR, 
        "   Destroying DotGraph...\n");
    dg_->save_graph();
    delete(dg_);
    dg_ = NULL;
}

void SignalGraph::delete_signals_map() {

    // Create a signals map iterator
    sig_map_t::iterator sig_map_it;
    
    // Iterate over the map using iterator till end
    while (!signals_map_.empty()) {   
        
        // Get refence to first map item
        sig_map_it = signals_map_.begin();

        // Delete signal
        delete(sig_map_it->second);
        sig_map_it->second = NULL;

        // Remove signal from map
        signals_map_.erase(sig_map_it->first);
    }
}

void SignalGraph::delete_connections_queue(conn_q_t* conn_q) {

    // Connection pointer
    Connection* conn;

    // Delete each connection in queue
    while (!conn_q->empty()) {

        // Get pointer to last connection in queue
        conn = conn_q->back();

        // delete connection
        delete(conn);
        conn = NULL;

        // remove connection from queue
        conn_q->pop_back();
    }

    // Delete connections queue
    delete(conn_q);
    conn_q = NULL;
}

void SignalGraph::delete_connections_map() {
    
    // Create connections map iterator
    conn_map_t::iterator conn_map_it;

    // Iterate over the map using iterator till end
    while (!connections_map_.empty()) {

        // Get reference to first map item   
        conn_map_it = connections_map_.begin();

        // Delete connections queue
        delete_connections_queue(conn_map_it->second);

        // Remove item from connections map
        connections_map_.erase(conn_map_it->first);
    }
} 

// ------------------------------------------------------------
// ------------------- Signal Enumeration ---------------------
// ------------------------------------------------------------

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

        // Check if signal already exists in map
        Error::check_signal_exists_in_map(signals_map_, current_signal);

        // Add signal to graph
        add_signal(current_signal);
    } 
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

void SignalGraph::add_signal(ivl_signal_t signal) {

    // Add signal to map
    signals_map_[signal] = new Signal(signal);

    // Check if arrayed signal has negative array base
    // @TODO: support arrayed signals with negative bases
    Error::check_arrayed_signal(signals_map_, signal);

    // Check if signal is multi-dimensional
    // @TODO: support multi-dimensional signals 
    // (i.e. signals packed dimensions like memories)
    Error::check_signal_not_multidimensional(signals_map_, signal);

    // Check if signal is to be ignored
    if (!check_if_ignore_signal(signal)) {

        // Initialize signal connections queue
        // Ignore local (IVL) generated signals.
        if (!ivl_signal_local(signal)) {
            // Intialize connections queue for signal
            connections_map_[signals_map_[signal]] = new conn_q_t();

            // Increment Signals Counter
            num_signals_++;
        } else {

            // Increment Local Signals Counter
            num_local_signals_++;
        }
    } else {
        num_signals_ignored_++;
    }
}

// ------------------------------------------------------------
// ----------------- Connection Enumeration -------------------
// ------------------------------------------------------------

bool SignalGraph::check_if_connection_exists(
    Signal*     sink_signal, 
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

bool SignalGraph::check_if_local_connection_exists(
    Signal*     sink_signal, 
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

bool SignalGraph::add_connection(
    Signal*        sink_signal, 
    Signal*        source_signal, 
    signal_slice_t sink_slice,
    signal_slice_t source_slice,
    string         ws) {

    Connection* conn;

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

        // Do not process connections to source signals to be ignored
        if (!check_if_ignore_signal(source_signal)) {

            // Do not process connections to ivl-generated source signals
            if (!source_signal->is_ivl_generated()) {

                // Create connection object
                conn = new Connection(source_signal, sink_signal, source_slice, sink_slice);

                // Check if connection already exists
                if (!check_if_connection_exists(sink_signal, conn)) {
                    
                    // Add node to graph if it is a CONSTANT
                    if (source_signal->is_const()) {
                        dg_->add_node(source_signal, ws);
                        num_constants_++;
                    }

                    // Add connection to dot graph
                    dg_->add_connection(conn, ws);
                    
                    // Save Connection
                    connections_map_[sink_signal]->push_back(conn);

                    // Increment connection counter
                    num_connections_++;  

                } else {
                    fprintf(DEBUG_PRINTS_FILE_PTR, "%sconnection (from %s to %s) already exists...\n", 
                        ws.c_str(),
                        conn->get_source()->get_fullname().c_str(),
                        conn->get_sink()->get_fullname().c_str());
                }

                return true;
            } else {
                return false;
            }
        }

    } else {

        Error::connecting_signal_not_in_graph(
            signals_map_, source_signal->get_ivl_signal());

    }

    return false;
}

void SignalGraph::track_local_signal_connection(
    Signal*        sink_signal, 
    Signal*        source_signal,
    signal_slice_t sink_slice,
    signal_slice_t source_slice,
    string         ws) {

    Connection* conn;

    // Check that source signal is valid (not null)
    assert(source_signal && 
        "ERROR: invalid (NULL) pointer to source signal.\n");

    // Check that sink signal is valid (not null)
    assert(sink_signal && 
        "ERROR: invalid (NULL) pointer to sink signal.\n");

    // Check that sink signal is NOT a constant
    assert(!sink_signal->is_const() && 
        "ERROR: sink signal cannot be a constant.\n");

    fprintf(DEBUG_PRINTS_FILE_PTR, "%sTracking local signal connection.\n", ws.c_str());

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

        // Do not process connections between TWO ivl-generated source signals
        if (source_signal->is_ivl_generated() || sink_signal->is_ivl_generated()) {

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

                fprintf(DEBUG_PRINTS_FILE_PTR, "%slocal connection (from %s to %s) already exists...\n", 
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

void SignalGraph::process_local_connections(string ws) {
    // Signal and Connection pointers
    Signal*     sink_signal  = NULL;
    Signal*     local_signal = NULL;
    Connection* current_conn = NULL;

    // Sink and Local connection references
    conn_map_t::iterator sink_conns_it;
    conn_map_t::iterator local_conns_it;

    // Get reference to non-local (sink) signal connections
    sink_conns_it = local_connections_map_.begin();

    // Process the map until its empty
    while (!local_connections_map_.empty() && 
           sink_conns_it != local_connections_map_.end()) {
        
        // Start at (non-local) sink signals
        if (!sink_conns_it->first->is_ivl_generated()) {

            // Get sink signal
            sink_signal = sink_conns_it->first;

            // Check that only one connection to sink signal exists
            assert(sink_conns_it->second->size() == 1 &&
                "ERROR: invalid local connection to sink signal.\n");

            // Get local (middle-man) signal
            local_signal = sink_conns_it->second->back()->get_source();

            // Check that the (one) connection to sink signal is local
            assert(local_signal->is_ivl_generated() &&
                "ERROR: signal connected to sink signal is NOT local.\n");

            // Delete sink connections queue
            sink_conns_it->second->pop_back();
            delete(sink_conns_it->second);
            sink_conns_it->second = NULL;

            // Remove sink signal from connections map
            local_connections_map_.erase(sink_signal);

            // Get reference to local (middle-man) signal connections
            local_conns_it = local_connections_map_.find(local_signal);
 
            // Iterate over source signals connected to local signal
            while (!local_conns_it->second->empty()) {
                
                // Get current connection obj
                current_conn = local_conns_it->second->back();

                // Modify sink signal of connection obj
                current_conn->set_sink(sink_signal);

                // Add connection to dot graph
                dg_->add_connection(current_conn, ws);
                num_connections_++;

                // Remove connection from queue after it is processed
                local_conns_it->second->pop_back();

                // Delete connection obj
                delete(current_conn);
                current_conn = NULL;
            }

            // Delete local signal connections queue
            delete(local_conns_it->second);
            local_conns_it->second = NULL;
            
            // Remove local signal from connections map
            local_connections_map_.erase(local_signal);

            // Reset sink signal connections reference
            sink_conns_it = local_connections_map_.begin();
        
        } else {
            
            // Increment sink signal connections reference
            sink_conns_it++;
        }
    }

    // Check all local connections have been processed
    assert(!local_connections_map_.size() && 
        "ERROR: some local connections remain unprocessed.\n");
}

// ------------------------------------------------------------
// ---------------------- Stats Counters ----------------------
// ------------------------------------------------------------

unsigned long SignalGraph::get_num_connections() const {
    return num_connections_;
}

unsigned long SignalGraph::get_num_signals() const {
    return num_signals_;
}

unsigned long SignalGraph::get_num_local_signals() const {
    return num_local_signals_;
}

unsigned long SignalGraph::get_num_constants() const {
    return num_constants_;
}

// ------------------------------------------------------------
// ------------------------ Signal Map ------------------------
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// ------------------- Dot Graph Management -------------------
// ------------------------------------------------------------

void SignalGraph::write_signals_to_dot_graph() {
    
    // Check that number of signals is correct
    assert((num_signals_ + num_local_signals_ + num_signals_ignored_) == 
        signals_map_.size() && 
        "ERROR: number of signals does not match size of signals map.\n");

    sig_map_t::iterator sig_it = signals_map_.begin();

    while (sig_it != signals_map_.end()) {
        
        // Check if Signal is an IVL signal
        if (sig_it->second->is_signal()) {

            // Check if signal is to be ignored
            if (!check_if_ignore_signal(sig_it->second)) {
    
                // Check if signal is local (IVL generated)
                if (!sig_it->second->is_ivl_generated()) {

                    // Iterate over all words in signal
                    for (unsigned int i = sig_it->second->get_array_base(); 
                                      i < sig_it->second->get_array_count(); 
                                      i++) {

                        // Set ID of signal
                        sig_it->second->set_id(i);

                        // Add signal to dot graph
                        dg_->add_node(sig_it->second, "");   
                    }
                }
            }
        }
        
        // Increment iterator
        sig_it++;
    }
}

void SignalGraph::save_dot_graph() {
    dg_->save_graph();
}

// ------------------------------------------------------------
// ---------------------- Config Loading ----------------------
// ------------------------------------------------------------

void SignalGraph::process_cmd_line_args(cmd_args_map_t* cmd_args) {
    
    // Load signals to ignore
    if (cmd_args->count(IGNORE_FILEPATH_FLAG)) {
        load_signals_to_ignore(cmd_args->at(IGNORE_FILEPATH_FLAG));
    }
}

void SignalGraph::add_signal_to_ignore(string signal_basename) {
    signals_to_ignore_[signal_basename] = true;
}

void SignalGraph::load_signals_to_ignore(string file_path) {

    fprintf(DEBUG_PRINTS_FILE_PTR, "Loading signal (base)names to ignore:\n");

    // Ignore file stream object
    ifstream file_stream = ifstream(file_path);

    // Signal basename (line) read from file stream
    string signal_basename;

    // Open file
    if (file_stream.is_open()) {

        // Process File
        while (getline(file_stream, signal_basename)) {
            signals_to_ignore_[signal_basename] = true;
            fprintf(DEBUG_PRINTS_FILE_PTR, "%s%s\n", WS_TAB, signal_basename.c_str());
        }

        // Close File
        file_stream.close();

    } else {
        fprintf(stderr, "ERROR: Could not open file %s\n", file_path.c_str());
        exit(FILE_ERROR);
    }
} 