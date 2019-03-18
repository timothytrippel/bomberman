/*
File:        propagate_signal.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signals connected to other SIGNALS.
This usually happens on module hookups. In order to create
a directed graph, we need to know what signals are inputs/
outputs of module so we know the direction of information flow.
*/

// Standard Headers
#include <cassert>
#include <cstdio>

// TTB Headers
#include "ttb_typedefs.h"
#include "ttb.h"
#include "error.h"

const char* get_signal_port_type_as_string(ivl_signal_t signal) {
	switch (ivl_signal_port(signal)) {
		case IVL_SIP_NONE:
			return "IVL_SIP_NONE";
		case IVL_SIP_INPUT:
			return "IVL_SIP_INPUT";
		case IVL_SIP_OUTPUT:
			return "IVL_SIP_OUTPUT";
		case IVL_SIP_INOUT:
			return "IVL_SIP_INOUT";
		default:
			return "UNKOWN";
	}
} 

// unsigned int get_signal_scope_depth(ivl_signal_t signal) {
	
// 	// Scope depth counter
// 	unsigned int depth = 0;

// 	// Curren scope
// 	ivl_scope_t current_scope = ivl_signal_scope(signal);

// 	// Increment depth counter until parent 
// 	// of current scope is NULL
// 	while ((current_scope = ivl_scope_parent(current_scope))) {

// 		// Increment depth counter
// 		depth++;
// 	}

// 	return depth;
// }

bool is_signal_in_parent_scope(
	ivl_signal_t child_signal, 
	ivl_signal_t parent_signal) {

	// Get signal scopes
	ivl_scope_t child_signal_scope  = ivl_signal_scope(child_signal);
	ivl_scope_t parent_signal_scope = ivl_signal_scope(parent_signal);
	
	// if (child_signal_scope && parent_signal_scope && ivl_scope_parent(child_signal_scope)) {
	// 	fprintf(stdout, "Child Scope: %s, Parent Scope: %s, Parent-of-Child Scope: %s\n", 
	// 		ivl_scope_basename(child_signal_scope),
	// 		ivl_scope_basename(parent_signal_scope),
	// 		ivl_scope_basename(ivl_scope_parent(child_signal_scope)));
	// } else if (child_signal_scope && parent_signal_scope) {
	// 	fprintf(stdout, "Child Scope: %s, Parent Scope: %s, Parent-of-Child Scope: %s\n", 
	// 		ivl_scope_basename(child_signal_scope),
	// 		ivl_scope_basename(parent_signal_scope),
	// 		"NULL");
	// } else if (child_signal_scope && ivl_scope_parent(child_signal_scope)) {
	// 	fprintf(stdout, "Child Scope: %s, Parent Scope: %s, Parent-of-Child Scope: %s\n", 
	// 		ivl_scope_basename(child_signal_scope),
	// 		"NULL",
	// 		ivl_scope_basename(ivl_scope_parent(child_signal_scope)));
	// } else if (parent_signal_scope && ivl_scope_parent(child_signal_scope)) {
	// 	fprintf(stdout, "Child Scope: %s, Parent Scope: %s, Parent-of-Child Scope: %s\n", 
	// 		"NULL",
	// 		ivl_scope_basename(parent_signal_scope),
	// 		ivl_scope_basename(ivl_scope_parent(child_signal_scope)));
	// }

	// Check if parent of child scope is the parent scope
	if (ivl_scope_parent(child_signal_scope) == parent_signal_scope) {
		return true;
	} else {
		return false;
	}
}

void process_non_port_signal(
	ivl_signal_t source_signal,
	Signal* 	 sink_signal,
	SignalGraph* sg,
	string       ws) {

    // Check that slice-info stacks are correct sizes
    // Source Slices Stack:
    // (Source slice stack should never grow beyond size N, 
    //  where N = number of nodes on source signals queue.)
    Error::check_slice_tracking_stack(sg->get_num_source_slices(), 1);
    // Sink Slices Stack:
    // (Sink slice stack should never grow beyond size 1.)
    Error::check_slice_tracking_stack(sg->get_num_sink_slices(), 1);

    // Add Connection
    sg->add_connection(
        sink_signal, 
        sg->get_signal_from_ivl_signal(source_signal), 
        ws);
}

void process_input_port_signal(
	ivl_signal_t source_signal,
	Signal* 	 sink_signal,
	SignalGraph* sg,
	string       ws) {

	// Get sink signal port direction
	switch (ivl_signal_port(sink_signal->get_ivl_signal())) {
		case IVL_SIP_NONE:
		case IVL_SIP_OUTPUT:
			process_non_port_signal(source_signal, sink_signal, sg, ws);
			break;
		case IVL_SIP_INPUT:

			// Ignore connection if scope of source signal is equal 
			// or deeper than the scope of the sink signal.
			if (is_signal_in_parent_scope(sink_signal->get_ivl_signal(), source_signal)) {

				// Process signal to signal connection
				process_non_port_signal(source_signal, sink_signal, sg, ws);

			} else {

				// Ignore signal to signal connection
				fprintf(stdout, "%signoring connection between two input ports...\n", ws.c_str());	
			}
			
			break;
		case IVL_SIP_INOUT:
			Error::not_supported("sink signal port type (IVL_SIP_INOUT).");
			break;
		default:
			Error::unknown_signal_port_type(ivl_signal_port(sink_signal->get_ivl_signal()));
        	break;
	}
}

void process_output_port_signal(
	ivl_signal_t source_signal,
	Signal* 	 sink_signal,
	SignalGraph* sg,
	string       ws) {

	// Get sink signal port direction
	switch (ivl_signal_port(sink_signal->get_ivl_signal())) {
		case IVL_SIP_NONE:
		case IVL_SIP_INPUT:
			process_non_port_signal(source_signal, sink_signal, sg, ws);
			break;
		case IVL_SIP_OUTPUT:
			
			// Ignore connection if scope of source signal is equal 
			// or deeper than the scope of the sink signal.
			if (is_signal_in_parent_scope(source_signal, sink_signal->get_ivl_signal())) {

				// Process signal to signal connection
				process_non_port_signal(source_signal, sink_signal, sg, ws);

			} else {

				// Ignore signal to signal connection
				fprintf(stdout, "%signoring connection between two input ports...\n", ws.c_str());	
			}
			
			break;
		case IVL_SIP_INOUT:
			Error::not_supported("sink signal port type (IVL_SIP_INOUT).");
			break;
		default:
			Error::unknown_signal_port_type(ivl_signal_port(sink_signal->get_ivl_signal()));
        	break;
	}
}

void propagate_signal(
	ivl_signal_t source_signal,
	Signal* 	 sink_signal,
	SignalGraph* sg,
	string       ws) {

	// Check that source signal is different from sink signal
	if (source_signal != sink_signal->get_ivl_signal()) {
		
		// Get source signal port direction
		switch (ivl_signal_port(source_signal)) {
			case IVL_SIP_NONE:
				process_non_port_signal(source_signal, sink_signal, sg, ws + WS_TAB);
				break;
			case IVL_SIP_INPUT:
				process_input_port_signal(source_signal, sink_signal, sg, ws + WS_TAB);
				break;
			case IVL_SIP_OUTPUT:
				process_output_port_signal(source_signal, sink_signal, sg, ws + WS_TAB);
				break;
			case IVL_SIP_INOUT:
				Error::not_supported("source signal port type (IVL_SIP_INOUT).");
				break;
			default:
				Error::unknown_signal_port_type(ivl_signal_port(source_signal));
            	break;
		}
	}
}
