/*
File:        tracker.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <cassert>

// TTB Headers
#include <ttb_typedefs.h>
#include <tracker.h>
#include <error.h>

// ------------------------------------------------------------
// ------------------------ Constructors ----------------------
// ------------------------------------------------------------

Tracker::Tracker():
	ignore_constants_(false), 
	clk_basename_(""), 
	inside_ff_block_(false),
	slicing_enabled_(false),
	array_index_is_signal_(false),
	sg_(NULL),
	source_signals_(),
	explored_nexi_(),
	num_signals_at_depth_() {}

Tracker::Tracker(
	cmd_args_map_t* cmd_args,
	SignalGraph*    sg) {

	// Intialize Flags
    inside_ff_block_        = false;
    slicing_enabled_        = false;
    array_index_is_signal_  = false;
    ignore_constants_       = false;

    // Intialize Data
    clk_basename_ = string("");
    sg_           = sg;

    // Process command line args
    process_cmd_line_args(cmd_args);
}

// ------------------------------------------------------------
// ------------------------ Destructors -----------------------
// ------------------------------------------------------------

Tracker::~Tracker() {

	// 1. Set pointer to SignalsGraph to NULL
	sg_ = NULL;

	// 2. Check that all explored signals processed
	assert(explored_nexi_.size() == 0 &&
		"ERROR-Tracker::~Tracker: nexi left unprocessed.\n");

	// 3. Check that all source signals processed
	assert(source_signals_.get_num_signals() == 0 &&
		"ERROR-Tracker::~Tracker: source signals left unprocessed.\n");
}

// ------------------------------------------------------------
// ----------------- Flip-Flop Status Tracking ----------------
// ------------------------------------------------------------

void Tracker::set_inside_ff_block() {
    inside_ff_block_ = true;
}

void Tracker::clear_inside_ff_block() {
    inside_ff_block_ = false;
}

bool Tracker::check_if_inside_ff_block() const {
    return inside_ff_block_;
}

bool Tracker::check_if_clk_signal(ivl_signal_t source_signal) const {
    if (string(ivl_signal_basename(source_signal)).find(clk_basename_) != string::npos) {
        return true;
    } else {
        return false;
    }
    // if (strcmp(clk_basename_.c_str(), ivl_signal_basename(source_signal))) {
    //     return false;
    // } else {
    //     return true;
    // }
}

// ------------------------------------------------------------
// ------------------ Source Signal Tracking ------------------
// ------------------------------------------------------------

Signal* Tracker::get_source_signal(unsigned int index) const {
    return source_signals_.get_signal(index);
}

Signal* Tracker::pop_source_signal(string ws __attribute__((unused))) {

	DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "%spopping %d source signal(s) from stack\n", 
        ws.c_str(), 1);)

	// Pop source signal
    return source_signals_.pop_signal();
}

void Tracker::pop_source_signals(unsigned int num_signals, string ws __attribute__((unused))) {

	DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "%spopping %d source signal(s) from stack\n", 
        ws.c_str(), num_signals);)
	
	// Pop source signals
	source_signals_.pop_signals(num_signals);
}

void Tracker::push_source_signal(
	Signal*      source_signal, 
	unsigned int id,
	string       ws __attribute__((unused))) {

    // Check that source signal is valid (not NULL)
    assert(source_signal && 
    	"ERROR: attempting to push NULL source signal to queue.\n");

    // Debug Print
    DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "%spushing source signal (name: %s, ID: %u) to stack\n", 
        ws.c_str(), 
        source_signal->get_fullname().c_str(),
        id);)

    // Push source signal to stack
    source_signals_.push_signal(source_signal, id);
}

// ------------------------------------------------------------
// ---------------------- Slice Tracking ----------------------
// ------------------------------------------------------------

void Tracker::enable_slicing() {
	slicing_enabled_ = true;
}

void Tracker::disable_slicing() {
	slicing_enabled_ = false;	
}

void Tracker::set_source_slice(
	Signal*      signal, 
	unsigned int msb, 
	unsigned int lsb, 
	string       ws) {

	// Check if slice tracking enabled
	if (slicing_enabled_) {
		signal->set_source_slice(msb, lsb, ws);
	}
}

void Tracker::set_sink_slice(
	Signal*      signal, 
	unsigned int msb, 
	unsigned int lsb, 
	string       ws) {

	// Check if slice tracking enabled
	if (slicing_enabled_) {
		signal->set_sink_slice(msb, lsb, ws);
	}
}

void Tracker::set_source_slice(
	Signal*        signal, 
	signal_slice_t slice, 
	string         ws) {

	// Check if slice tracking enabled
	if (slicing_enabled_) {
		signal->set_source_slice(slice, ws);
	}
}

void Tracker::set_sink_slice(
	Signal*        signal, 
	signal_slice_t slice, 
	string         ws) {

	// Check if slice tracking enabled
	if (slicing_enabled_) {
		signal->set_sink_slice(slice, ws);
	}
}

void Tracker::shift_source_slice(
	Signal* signal, 
	int     num_bits, 
	string  ws) {

	// Check if slice tracking enabled
	if (slicing_enabled_) {
		signal->shift_source_slice(num_bits, ws);
	}
}

void Tracker::shift_sink_slice(
	Signal* signal, 
	int     num_bits, 
	string  ws) {

	// Check if slice tracking enabled
	if (slicing_enabled_) {
		signal->shift_sink_slice(num_bits, ws);
	}
}

// ------------------------------------------------------------
// ---------------------- Depth Tracking ----------------------
// ------------------------------------------------------------

unsigned int Tracker::pop_scope_depth() {

    // Check that scope depth queue is not empty
    assert(num_signals_at_depth_.size() > 0 && 
    	"ERROR-Tracker::pop_scope_depth: cannot pop from empty queue.\n");

    // Get last node in queue
    unsigned int num_signals = num_signals_at_depth_.back();

    // Remove last node in queue
    num_signals_at_depth_.pop_back();

    // Return removed node
    return num_signals;
}

void Tracker::push_scope_depth(unsigned int num_signals) {
    num_signals_at_depth_.push_back(num_signals);
}

// ------------------------------------------------------------
// ---------------------- Config Loading ----------------------
// ------------------------------------------------------------

void Tracker::process_cmd_line_args(cmd_args_map_t* cmd_args) {
    
    // Initialize CLK Basename
    clk_basename_ = cmd_args->at(CLK_BASENAME_FLAG);

    // // Load ignore constants flag
    // if (!strcmp(cmd_args->at(IGNORE_CONSTANTS_FLAG).c_str(), "yes")) {
    //     ignore_constants_ = true;
    // }
}
