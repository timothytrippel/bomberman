/*
File:        signal.cc
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
#include "signal.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------
Signal::Signal():
	ivl_object_(),
	ivl_type_(IVL_NONE) {}

// Signal
Signal::Signal(ivl_signal_t signal):
	ivl_object_(signal),
	ivl_type_(IVL_SIGNAL) {}

// Net Constant
Signal::Signal(ivl_net_const_t constant):
	ivl_object_(constant),
	ivl_type_(IVL_CONST) {}

// Constant Expression
Signal::Signal(ivl_expr_t expression):
	ivl_object_(expression),
	ivl_type_(IVL_EXPR) {}

// ----------------------------------------------------------------------------------
// ---------------------------------- Getters ---------------------------------------
// ----------------------------------------------------------------------------------
void* Signal::get_ivl_obj() const {
	switch(ivl_type_) {
        case IVL_SIGNAL:
            return ivl_object_.ivl_signal;
        case IVL_CONST:
            return ivl_object_.ivl_const;
        case IVL_EXPR:
            return ivl_object_.ivl_expr;
        default:
            return NULL;
    }
}

ivl_obj_type_t Signal::get_ivl_type() const {
	return ivl_type_;
}

ivl_signal_t Signal::get_ivl_signal() const {
	if (ivl_type_ == IVL_SIGNAL) {
		return ivl_object_.ivl_signal;
	} else {
		return NULL;
	}
}

string Signal::get_fullname() const {
    switch(ivl_type_) {
        case IVL_SIGNAL:
            return get_signal_fullname();
        case IVL_CONST:
            return get_const_fullname();
        case IVL_EXPR:
            return get_expr_fullname();
        default:
            return "NONE";
    }
}

unsigned int Signal::get_msb() const {
	switch(ivl_type_) {
        case IVL_SIGNAL:
            return get_signal_msb();
        case IVL_CONST:
            return get_const_msb();
        case IVL_EXPR:
            return get_expr_msb();
        default:
            return 0;
    }
}

unsigned int Signal::get_lsb() const {
	switch(ivl_type_) {
        case IVL_SIGNAL:
            return get_signal_lsb();
        case IVL_CONST:
            return 0;
        case IVL_EXPR:
            return 0;
        default:
            return 0;
    }
}

// ----------------------------------- Signal ---------------------------------------
string Signal::get_signal_fullname() const {
    string scopename = ivl_scope_name(ivl_signal_scope(ivl_object_.ivl_signal)); 
    string basename  = ivl_signal_basename(ivl_object_.ivl_signal);
    string fullname  = scopename + string(".") + basename;

    return fullname;
}

unsigned int Signal::get_signal_msb() const {
    if (ivl_signal_packed_dimensions(ivl_object_.ivl_signal) > 0) {
        // Check MSB is not negative
        assert((ivl_signal_packed_msb(ivl_object_.ivl_signal, 0) >= 0) && \
            "NOT-SUPPORTED: negative MSB index.\n");
        
        return ivl_signal_packed_msb(ivl_object_.ivl_signal, 0);
    } else {
        return 0;
    }
}

unsigned int Signal::get_signal_lsb() const {
    if (ivl_signal_packed_dimensions(ivl_object_.ivl_signal) > 0) {
        // Check LSB is not negative
        assert((ivl_signal_packed_lsb(ivl_object_.ivl_signal, 0) >= 0) && \
            "NOT-SUPPORTED: negative LSB index.\n");

        return ivl_signal_packed_lsb(ivl_object_.ivl_signal, 0);
    } else {
        return 0;
    }
}

// ---------------------------------- Constant --------------------------------------
string Signal::get_const_fullname() const {
    // string scopename = ivl_scope_name(ivl_const_scope(ivl_object_.ivl_const)); 
    string basename  = string(
    	ivl_const_bits(ivl_object_.ivl_const), 
    	(size_t)ivl_const_width(ivl_object_.ivl_const));
    reverse(basename.begin(), basename.end());
    // string fullname  = scopename + string(".const_") + basename;
    string fullname  = string("const_") + basename;

    return fullname;
}

unsigned int Signal::get_const_msb() const {
	// Check MSB is not negative
    assert((ivl_const_width(ivl_object_.ivl_const) >= 0) && \
        "NOT-SUPPORTED: negative MSB index.\n");

    return ivl_const_width(ivl_object_.ivl_const) - 1;
}

// --------------------------------- Expression -------------------------------------
string Signal::get_expr_fullname() const {
    string basename  = string(
    	ivl_expr_bits(ivl_object_.ivl_expr), 
    	(size_t)ivl_expr_width(ivl_object_.ivl_expr));
    reverse(basename.begin(), basename.end());
    string fullname  = string("const_") + basename;

    return fullname;
}

unsigned int Signal::get_expr_msb() const {
    return ivl_expr_width(ivl_object_.ivl_expr) - 1;
}

// ----------------------------------------------------------------------------------
// -------------------------------Dot Graph Getters ---------------------------------
// ----------------------------------------------------------------------------------
string Signal::get_dot_label() const {
	switch(ivl_type_) {
        case IVL_SIGNAL:
            return get_dot_signal_label();
        case IVL_CONST:
            return get_dot_const_label();
        case IVL_EXPR:
            return get_dot_expr_label();
        default:
            return "?";
    }
}

string Signal::get_dot_shape() const {
	switch(ivl_type_) {
        case IVL_CONST:
        case IVL_EXPR:
            return CONST_NODE_SHAPE;
        case IVL_SIGNAL:
        default:
            return SIGNAL_NODE_SHAPE;
    }
}

// ----------------------------------- Signal ---------------------------------------
string Signal::get_dot_signal_label() const {
    stringstream ss;

    ss << "[";
    ss << get_signal_msb();
    ss << ":";
    ss << get_signal_lsb();
    ss << "]";

    return ss.str();
}

// ---------------------------------- Constant --------------------------------------
string Signal::get_dot_const_label() const {
    stringstream ss;

    ss << "[";
    ss << get_const_msb();
    ss << ":0]";

    return ss.str();
}

// --------------------------------- Expression -------------------------------------
string Signal::get_dot_expr_label() const {
    stringstream ss;

    ss << "[";
    ss << get_expr_msb();
    ss << ":0]";

    return ss.str();
}

// ----------------------------------------------------------------------------------
// ----------------------------------- Other ----------------------------------------
// ----------------------------------------------------------------------------------
bool Signal::is_ivl_generated() const {
	if (ivl_type_ == IVL_SIGNAL) {
		return ivl_signal_local(ivl_object_.ivl_signal);
	} else {
		return false;
	}
}