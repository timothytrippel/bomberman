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

// Standard Headers
#include <string>
#include <vector>
#include <map>

using namespace std;

// IVL API Header
#include  <ivl_target.h>

// Debugging Switches
#define DEBUG_PRINTS false

// Progress Messages
#define LAUNCH_MESSAGE          "Entering TTB Target Module..."
#define SCOPE_EXPANSION_MESSAGE "Identifying top-level modules..."

// Signal name to signal object map
typedef map<ivl_signal_t, string> sig_name_map_t;

// Signal graph adjaceny list
typedef map<ivl_signal_t, vector<ivl_signal_t>*> sig_map_t;

// Signal to signal connection pair
// typedef pair<ivl_signal_t*, ivl_signal_t*> connection_t;

// Functions
ivl_net_const_t is_const_local_sig(ivl_signal_t sig);
void find_signals(ivl_scope_t scope, sig_name_map_t& signal_to_name, sig_map_t& signals);
void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes, sig_name_map_t& signal_to_name, sig_map_t& signals);
void find_all_connections(sig_map_t& signals);

#endif
