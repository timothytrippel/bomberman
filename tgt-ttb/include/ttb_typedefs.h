/*
File:        ttb_typedefs.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_TYPEDEFS_HEADER__
#define __TTB_TYPEDEFS_HEADER__

// Standard Headers
#include <vector>
#include <map>

using namespace std;

// IVL API Header
#include  <ivl_target.h>

// Progress Messages
#define LAUNCH_MESSAGE          "Entering TTB Target Module..."
#define SCOPE_EXPANSION_MESSAGE "Identifying top-level modules..."
#define SIGNAL_ENUM_MESSAGE     "Enumerating all signals..."
#define CONNECTION_ENUM_MESSAGE "Enumerating all signal-to-signal connections..."

// Signal graph adjaceny list
typedef map<ivl_signal_t, vector<ivl_signal_t>> sig_map_t;

// Node that is sliced (either SINK or SOURCE)
typedef enum SLICE_NODE {
      SINK   = 0,
      SOURCE = 1
} slice_node_t;

// Struct for holding MSB-LSB pair for tracking 
// signal vector slices at a given nexus
struct SliceInfo {
	unsigned int msb;
	unsigned int lsb;
	slice_node_t node;
};

#endif
