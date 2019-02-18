/*
File:        ttb.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_HEADER__
#define __TTB_HEADER__

using namespace std;

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"

// Debugging Switches
#define DEBUG_PRINTS false

// Progress Messages
#define LAUNCH_MESSAGE          "Entering TTB Target Module..."
#define SCOPE_EXPANSION_MESSAGE "Identifying top-level modules..."
#define SIGNAL_ENUM_MESSAGE     "Enumerating all signals..."
#define CONNECTION_ENUM_MESSAGE "Enumerating all signal-to-signal connections..."

// Functions
void find_signals(ivl_scope_t scope, sig_name_map_t& signal_to_name, sig_map_t& signals, DotGraph dg);
void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes, sig_name_map_t& signal_to_name, sig_map_t& signals, DotGraph dg);
void find_all_connections(sig_map_t& signals, DotGraph dg);

#endif
