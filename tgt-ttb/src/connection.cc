/*
File:        connection.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <sstream>

// TTB Headers
#include "connection.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------
Connection::Connection():
    type_(NO_CONNECTION),
	source_(),
	source_msb_(0),
    source_lsb_(0),
    sink_(),
    sink_msb_(0),
    sink_lsb_(0) {}

Connection::Connection(Signal source, Signal sink):
    type_(FULL_CONNECTION),
    source_(source),
    source_msb_(source.get_msb()),
    source_lsb_(source.get_lsb()),
    sink_(sink),
    sink_msb_(sink.get_msb()),
    sink_lsb_(sink.get_lsb()) {}

Connection::Connection(Signal         source,
                       Signal         sink,
                       signal_slice_t source_slice,
                       signal_slice_t sink_slice):

    type_(BOTH_SLICE),
    source_(source),
    source_msb_(source_slice.msb),
    source_lsb_(source_slice.lsb),
    sink_(sink),
    sink_msb_(sink_slice.msb),
    sink_lsb_(sink_slice.lsb) {}

// ----------------------------------------------------------------------------------
// ---------------------------------- Getters ---------------------------------------
// ----------------------------------------------------------------------------------
Signal Connection::get_source() const {
    return source_;
}

Signal Connection::get_sink() const {
    return sink_;   
}

// ----------------------------------------------------------------------------------
// ------------------------------ Dot Graph Getters ---------------------------------
// ----------------------------------------------------------------------------------
string Connection::get_dot_label() const {
    stringstream ss;
    
    ss << "[";
    ss << source_msb_;
    ss << ":";
    ss << source_lsb_;
    ss << "]->[";
    ss << sink_msb_;
    ss << ":";
    ss << sink_lsb_;
    ss << "]";

    return ss.str();
}
