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
#include "ttb_typedefs.h"
#include "signal.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Constructors -------------------------------------
// ----------------------------------------------------------------------------------

Signal::Signal():
	ivl_object_(),
	ivl_type_(IVL_NONE),
	id_(0),
	is_ff_(false),
	is_input_(false),
	source_slice_modified_(false),
	sink_slice_modified_(false),
	source_msb_(0),
	source_lsb_(0),
	sink_msb_(0),
	sink_lsb_(0) {}

// Signal
Signal::Signal(ivl_signal_t signal):
	ivl_object_(signal),
	ivl_type_(IVL_SIGNAL),
	id_(0),
	is_ff_(false),
	is_input_(false),
	source_slice_modified_(false),
	sink_slice_modified_(false),
	source_msb_(0),
	source_lsb_(0),
	sink_msb_(0),
	sink_lsb_(0) {

	// Check if the signal is an input
	if (ivl_signal_port(signal) == IVL_SIP_INPUT) {
		set_is_input();
	}
}

// Net Constant
Signal::Signal(ivl_net_const_t constant):
	ivl_object_(constant),
	ivl_type_(IVL_CONST),
	id_(const_id),
	is_ff_(false),
	is_input_(false),
	source_slice_modified_(false),
	sink_slice_modified_(false),
	source_msb_(0),
	source_lsb_(0),
	sink_msb_(0),
	sink_lsb_(0) {

	// Increment Constant ID counter
	const_id++;
}

// Constant Expression
Signal::Signal(ivl_expr_t expression):
	ivl_object_(expression),
	ivl_type_(IVL_EXPR),
	id_(const_id),
	is_ff_(false),
	is_input_(false),
	source_slice_modified_(false),
	sink_slice_modified_(false),
	source_msb_(0),
	source_lsb_(0),
	sink_msb_(0),
	sink_lsb_(0) {

	// Increment Constant ID counter
	const_id++;
}

// ----------------------------------------------------------------------------------
// --------------------------------- Operators --------------------------------------
// ----------------------------------------------------------------------------------

bool Signal::operator==(const Signal& sig) const {
	
	if (this->ivl_type_ == sig.get_ivl_type()) {
		switch(this->ivl_type_) {
			case IVL_SIGNAL:
	            return (this->ivl_object_.ivl_signal == sig.get_ivl_obj().ivl_signal);
	        case IVL_CONST:
	            return (this->ivl_object_.ivl_const  == sig.get_ivl_obj().ivl_const);
	        case IVL_EXPR:
	            return (this->ivl_object_.ivl_expr   == sig.get_ivl_obj().ivl_expr);
	        default:
	            return false;
	    };
	} else {
		return false;
	}
}

bool Signal::operator!=(const Signal& sig) const {
	return !(*this == sig);
}

// ----------------------------------------------------------------------------------
// ---------------------------------- Getters ---------------------------------------
// ----------------------------------------------------------------------------------

ivl_object_t Signal::get_ivl_obj() const {
	switch(ivl_type_) {
        case IVL_SIGNAL:
            return ivl_object_.ivl_signal;
        case IVL_CONST:
            return ivl_object_.ivl_const;
        case IVL_EXPR:
            return ivl_object_.ivl_expr;
        default:
            return ivl_object_t();
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

string Signal::get_basename() const {
    switch(ivl_type_) {
        case IVL_SIGNAL:
            return get_signal_basename();
        case IVL_CONST:
            return get_const_bitstring();
        case IVL_EXPR:
            return get_expr_bitstring();
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
        case IVL_EXPR:
        default:
            return 0;
    }
}

unsigned int Signal::get_id() const {
	return id_;
}

unsigned int Signal::get_array_base() const {
	if (ivl_type_ == IVL_SIGNAL) {
		return ivl_signal_array_base(ivl_object_.ivl_signal);
	} else {
		return 0;
	}
}

unsigned int Signal::get_array_count() const {
	if (ivl_type_ == IVL_SIGNAL) {
		return ivl_signal_array_count(ivl_object_.ivl_signal);
	} else {
		return 1;
	}
}

bool Signal::is_signal() const {
	if (ivl_type_ == IVL_SIGNAL) {
		return true;
	} else {
		return false;
	}
}

bool Signal::is_const() const {
	if (ivl_type_ == IVL_CONST ||
		ivl_type_ == IVL_EXPR) {
		return true;
	} else {
		return false;
	}
}

bool Signal::is_arrayed() const {
	if (ivl_type_ == IVL_SIGNAL) {
		if (ivl_signal_dimensions(ivl_object_.ivl_signal) > 0) {
			return true;
		}
	}

	return false;
}

bool Signal::is_source_slice_modified() const {
	return source_slice_modified_;
}

bool Signal::is_sink_slice_modified() const {
	return sink_slice_modified_;
}

signal_slice_t Signal::get_source_slice(Signal* signal) const {

	// Check that signal is not NULL
	assert(signal && "ERROR-Signal::get_source_slice: cannot get slice of NULL signal.\n");

	// Check if tracking slice info
	if (source_slice_modified_) {
		return {source_msb_, source_lsb_};
	} else {

		// Check if this Signal is the SOURCE/SINK
		if (this == signal) {

			// This is the SOURCE signal
			return {this->get_msb(), this->get_lsb()};
		} else {

			// This is the SINK signal
			return {signal->get_msb(), signal->get_lsb()};
		}
	}
}

signal_slice_t Signal::get_sink_slice(Signal* signal) const {
	
	// Check that signal is not NULL
	assert(signal && "ERROR-Signal::get_sink_slice: cannot get slice of NULL signal.\n");

	// Check if tracking slice info
	if (sink_slice_modified_) {
		return {sink_msb_, sink_lsb_};
	} else {

		// Check if this Signal is the SOURCE/SINK
		if (this == signal) {

			// This is the SINK signal
			return {this->get_msb(), this->get_lsb()};
		} else {

			// This is the SOURCE signal
			return {signal->get_msb(), signal->get_lsb()};
		}
	}
}

// ----------------------------------- Signal ---------------------------------------

string Signal::get_signal_scopename() const {
    return ivl_scope_name(ivl_signal_scope(ivl_object_.ivl_signal));
}

string Signal::get_signal_basename() const {
    return ivl_signal_basename(ivl_object_.ivl_signal);
}

string Signal::get_signal_fullname() const {
	if (this->is_arrayed()) {
		return (get_signal_scopename() + 
			string(".") + 
			to_string(id_) + 
			string(".") + 
			get_signal_basename());	
	} else {
		return (get_signal_scopename() + string(".") + get_signal_basename());	
	}
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

string Signal::get_const_bitstring() const {

	// Get bitstring
	string bitstring = string(
    	ivl_const_bits(ivl_object_.ivl_const), 
    	(size_t) ivl_const_width(ivl_object_.ivl_const) );

	// Reverse bitstring to MSB->LSB order
    reverse(bitstring.begin(), bitstring.end());

    return bitstring;
}

string Signal::get_const_fullname() const {

    // Get (unique) constant prefix
    // string scopename = ivl_scope_name(ivl_const_scope(ivl_object_.ivl_const)); 
    string const_prefix = string("const.") + to_string(id_) + string(".");

    return (const_prefix + get_const_bitstring());
}

unsigned int Signal::get_const_msb() const {

	// Check MSB is not negative
    assert((ivl_const_width(ivl_object_.ivl_const) >= 0) && \
        "NOT-SUPPORTED: negative MSB index.\n");

    return ivl_const_width(ivl_object_.ivl_const) - 1;
}

// --------------------------------- Expression -------------------------------------

string Signal::get_expr_bitstring() const {

	// Get bitstring
	string bitstring = string(
    	ivl_expr_bits(ivl_object_.ivl_expr), 
    	(size_t) ivl_expr_width(ivl_object_.ivl_expr) );
    
	// Reverse bitstring to MSB->LSB order
	reverse(bitstring.begin(), bitstring.end());

	return bitstring;
}

string Signal::get_expr_fullname() const {

    // Get (unique) constant prefix
	string const_prefix = string("const.") + to_string(id_) + string(".");

    return (const_prefix + get_expr_bitstring());
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
        	if (is_ff_) {
        		return FF_NODE_SHAPE;
        	} else if (is_input_) {
        		return INPUT_NODE_SHAPE;
        	} else {
        		return SIGNAL_NODE_SHAPE;	
        	}
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
// ---------------------------------- Setters ---------------------------------------
// ----------------------------------------------------------------------------------

void Signal::set_is_ff() {
	
	// Check that it is a signal and of reg type
	assert(this->is_signal() && "ERROR: constants cannot be flagged as a FF.\n");
	assert((ivl_signal_type(ivl_object_.ivl_signal) == IVL_SIT_REG) && 
		"ERROR: non-reg type signals cannot be flagged as a FF.\n");

	// Set is_ff_
	is_ff_ = true;
}

void Signal::set_is_input() {
	
	// Check that it is a signal
	assert(this->is_signal() && "ERROR: constants cannot be flagged as an INPUT.\n");

	// Set is_input_
	is_input_ = true;
}

void Signal::set_id(unsigned int value) {
	
	// Check that it is a signal
	assert(this->is_signal() && "ERROR: ID of constants cannot be manipulated.\n");

	// Set the id_ value
	id_ = value;
}

void Signal::reset_slices() {
	source_msb_            = 0;
	source_lsb_            = 0; 
	sink_msb_              = 0; 
	sink_lsb_              = 0;
	source_slice_modified_ = false;
	sink_slice_modified_   = false;
}

void Signal::set_source_slice(unsigned int msb, unsigned int lsb, string ws) {

	// Debug Print
    fprintf(DEBUG_PRINTS_FILE_PTR, 
    	"%sSetting SOURCE signal slice [%u:%u]\n", 
        ws.c_str(), msb, lsb);

	source_slice_modified_ = true;
	source_msb_            = msb;
	source_lsb_            = lsb;
}

void Signal::set_sink_slice(unsigned int msb, unsigned int lsb, string ws) {

	// Debug Print
    fprintf(DEBUG_PRINTS_FILE_PTR, 
    	"%sSetting SINK signal slice [%u:%u]\n", 
        ws.c_str(), msb, lsb);

	sink_slice_modified_ = true;
	sink_msb_            = msb;
	sink_lsb_            = lsb;
}

void Signal::set_source_slice(signal_slice_t source_slice, string ws) {
	this->set_source_slice(source_slice.msb, source_slice.lsb, ws);
}

void Signal::set_sink_slice(signal_slice_t sink_slice, string ws) {
	this->set_sink_slice(sink_slice.msb, sink_slice.lsb, ws);
}

void Signal::update_source_slice(unsigned int msb, unsigned int lsb, string ws) {

	// Check that MSB and LSB are not negative
	assert(msb >= 0 && lsb >= 0 && 
		"ERROR-Signal::update_source_slice: cannot process negative slice indices.\n");

	// Check if tracking slice info
	if (source_slice_modified_) {

		// Debug Print
        fprintf(DEBUG_PRINTS_FILE_PTR, 
        	"%sUpdating SOURCE signal slice [%u:%u] with [%u:%u] to [%u:%u]\n", 
            ws.c_str(), 
            source_msb_, 
            source_lsb_, 
            msb, 
            lsb, 
            source_lsb_ + lsb + (msb - lsb),
            source_lsb_ + lsb);

		// ALREADY tracking it --> update slice
		source_lsb_ += lsb;
		source_msb_ = source_lsb_ + (msb - lsb);

	} else {

		// NOT tracking it yet --> record slice
		this->set_source_slice(msb, lsb, ws);
	}
}

void Signal::update_sink_slice(unsigned int msb, unsigned int lsb, string ws) {
	
	// Check that MSB and LSB are not negative
	assert(msb >= 0 && lsb >= 0 && 
		"ERROR-Signal::update_sink_slice: cannot process negative slice indices.\n");

	// Check if tracking slice info
	if (sink_slice_modified_) {

		// Debug Print
        fprintf(DEBUG_PRINTS_FILE_PTR, 
        	"%sUpdating SINK signal slice [%u:%u] with [%u:%u] to [%u:%u]\n", 
            ws.c_str(), 
            sink_msb_, 
            sink_lsb_, 
            msb, 
            lsb, 
            sink_lsb_ + lsb + (msb - lsb),
            sink_lsb_ + lsb);

		// ALREADY tracking it --> update slice
		sink_lsb_ += lsb;
		sink_msb_ = sink_lsb_ + (msb - lsb);

	} else {

		// NOT tracking it yet --> record slice
		this->set_sink_slice(msb, lsb, ws);
	}
}

void Signal::update_source_slice(signal_slice_t source_slice, string ws) {
	this->update_source_slice(source_slice.msb, source_slice.lsb, ws);
}

void Signal::update_sink_slice(signal_slice_t sink_slice, string ws) { 
	this->update_sink_slice(sink_slice.msb, sink_slice.lsb, ws);
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

unsigned int Signal::process_as_partselect_expr(ivl_statement_t statement) const {

    // Check part_select is only of type IVL_CONST_EXPR
    // @TODO: support non-constant part selects,
    // e.g. signals: signal_a[signal_b] <= signal_c;
    Error::check_part_select_expr(ivl_type_, statement);

    // Get LSB offset index
    string bit_string = string(ivl_expr_bits(ivl_object_.ivl_expr));
    reverse(bit_string.begin(), bit_string.end());

    // Convert bitstring to unsigned long
    return stoul(bit_string, NULL, BITSTRING_BASE);
}