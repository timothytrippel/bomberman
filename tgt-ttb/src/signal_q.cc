/*
File:        signal_q.cc
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
#include <signal_q.h>
#include <error.h>

// ------------------------------------------------------------
// ------------------------ Constructors ----------------------
// ------------------------------------------------------------
SignalQ::SignalQ():
	signal_q_(),
	id_q_() {}

// ------------------------------------------------------------
// -------------------------- Getters -------------------------
// ------------------------------------------------------------

unsigned int SignalQ::get_num_signals() const {
	return signal_q_.size();
}

Signal* SignalQ::get_signal(unsigned int index) const {

	// Check that queues are not empty, and are the same size
	assert(signal_q_.size() > 0 && 
		"ERROR-SignalQ::get_signal: cannot pop from empty signal queue.\n");
	assert(id_q_.size() > 0 && 
		"ERROR-SignalQ::get_signal: cannot pop from empty ID queue.\n");
	assert(signal_q_.size() == id_q_.size() && 
		"ERROR-SignalQ::get_signal: signal and ID queues should be the same size.\n");

	// Check that index is within the size bounds
	assert(index < signal_q_.size() &&
		"ERROR-SignalQ::get_signal: index outsize queue bounds.\n");

	// Signal/ID to return
	Signal*      signal = signal_q_[index];
	unsigned int id     = id_q_[index];

	// Set signal ID (arrayed source signals)
    if (signal->is_signal()) {
		signal->set_id(id);
	}

	return signal;
}

Signal* SignalQ::get_back_signal() const {
	return this->get_signal(signal_q_.size() - 1);
}

Signal* SignalQ::get_back_signal(unsigned int index) const {
	return this->get_signal(signal_q_.size() - index - 1);
}

Signal* SignalQ::pop_signal() {

	// Check that queues are not empty, and are the same size
	assert(signal_q_.size() > 0 && 
		"ERROR-SignalQ::pop_signal: cannot pop from empty signal queue.\n");
	assert(id_q_.size() > 0 && 
		"ERROR-SignalQ::pop_signal: cannot pop from empty ID queue.\n");
	assert(signal_q_.size() == id_q_.size() && 
		"ERROR-SignalQ::pop_signal: signal and ID queues should be the same size.\n");

	// Signal/ID to return
	Signal*      signal = signal_q_.back();
	unsigned int id     = id_q_.back();

	// Pop last items in each queue
	signal_q_.pop_back();
	id_q_.pop_back();

	// Set signal ID (arrayed source signals)
    if (signal->is_signal()) {
		signal->set_id(id);
	}

	return signal;
}

void SignalQ::pop_signals(unsigned int num_signals) {

	// Check that the queue is large enough
	assert(signal_q_.size() >= num_signals &&
		"ERROR-SignalQ::pop_slices: signal queue not large enough to pop <num_signals>.\n");

	// Pop signals/IDs
	for (unsigned int i = 0; i < num_signals; i++) {
		
		// Reset signal slices
    	signal_q_.back()->reset_slices();

    	// Remove items from queues
		signal_q_.pop_back();
		id_q_.pop_back();
	} 
}

// ------------------------------------------------------------
// -------------------------- Setters -------------------------
// ------------------------------------------------------------

void SignalQ::push_signal(Signal* signal, unsigned int id) {

	// Push slice to the queue
	signal_q_.push_back(signal);
	id_q_.push_back(id);
}

// ------------------------------------------------------------
// --------------------------- Debug --------------------------
// ------------------------------------------------------------

void SignalQ::print() const {
	for (unsigned int i = 0; i < signal_q_.size(); i++) {
		fprintf(stdout, "%s%s\n", WS_TAB, get_signal(i)->get_fullname().c_str());
	}
}
