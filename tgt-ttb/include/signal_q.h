/*
File:        signal_q.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __SIGNAL_Q_HEADER__
#define __SIGNAL_Q_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <vector>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "signal.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------- SignalQ --------------------------
// ------------------------------------------------------------

class SignalQ {
	public:
		// Constructors
		SignalQ();

		// Getters
		unsigned int get_num_signals() const;
		Signal*      get_signal(unsigned int index) const;
		Signal*      get_back_signal() const;
		Signal*      get_back_signal(unsigned int index) const;
		Signal*      pop_signal();
		void         pop_signals(unsigned int num_signals);

		// Setters
		void push_signal(Signal* signal, unsigned int id);

		// Debug
		void print() const;

	private:
		vector<Signal*>      signal_q_;
		vector<unsigned int> id_q_;
};

#endif
