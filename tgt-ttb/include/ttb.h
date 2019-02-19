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

// Functions
void find_signals(ivl_scope_t     scope, \
                  sig_name_map_t& signal_to_name, \
                  sig_map_t&      signals_map, \
                  DotGraph        dg);

void find_all_signals(ivl_scope_t*    scopes, \
                      unsigned int    num_scopes, \
                      sig_name_map_t& signal_to_name, \
                      sig_map_t&      signals_map, \
                      DotGraph        dg);

unsigned long find_all_connections(sig_map_t& signals_map, 
                                   DotGraph   dg);

unsigned long propagate_signal(ivl_signal_t connected_signal, \
                               ivl_signal_t signal, \
                               sig_map_t&   signals_map, \
                               DotGraph     dg);

unsigned long propagate_logic(ivl_net_logic_t logic_device, \
                              ivl_signal_t    signal, \
                              sig_map_t&      signals_map, \
                              DotGraph        dg);

unsigned long propagate_lpm(ivl_lpm_t    lpm_device, \
                            ivl_signal_t signal, \
                            sig_map_t&   signals_map, \
                            DotGraph     dg);

unsigned long propagate_constant(ivl_net_const_t constant, \
                                 ivl_signal_t    signal, \
                                 sig_map_t&      signals_map, \
                                 DotGraph        dg);

#endif
