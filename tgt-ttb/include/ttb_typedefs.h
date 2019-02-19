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

// Signal name to signal object map
typedef map<ivl_signal_t, const char*> sig_name_map_t;

// Signal graph adjaceny list
typedef map<ivl_signal_t, vector<ivl_signal_t>> sig_map_t;

#endif
