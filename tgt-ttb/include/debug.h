/*
File:        debug.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __DEBUG_HEADER__
#define __DEBUG_HEADER__

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"

class Debug {
	public:
		Debug();
		static void print_nexus_ptrs(ivl_nexus_t);
};

#endif
