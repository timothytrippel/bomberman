#ifndef __TTB_HEADER__
#define __TTB_HEADER__

/*
File:        ttb.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <map>
#include <string>

// IVL API Header
#include  <ivl_target.h>

// TTB Headers

using namespace std;

// Debugging Switches
#define DEBUG_PRINTS 		          false

// Signal name to object map
typedef map<string, ivl_signal_t*> sig_map_t;

// Signal to signal connection pair
typedef pair<ivl_signal_t*, ivl_signal_t*> connection_t;

// Gets the output file set in ttb.cc
FILE* out_file();

#endif
// 