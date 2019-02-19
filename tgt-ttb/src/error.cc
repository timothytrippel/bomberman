/*
File:        error.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <cstdio>
#include <cassert>
#include <cstring>
#include <string>

// TTB Headers
#include "error.h"

using namespace std;

Error::Error(){}

void Error::check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes){
	ivl_scope_type_t scope_type           = IVL_SCT_MODULE;
	string           scope_type_name      = "UNKNOWN";
	bool             scope_type_supported = false;

	for (unsigned int i = 0; i < num_scopes; i++){
		// Get scope type
		scope_type = ivl_scope_type(scopes[i]);

		// Check scope is of type "module"
		assert(scope_type == IVL_SCT_MODULE && "UNSUPPORTED: Verilog scope type.\n");

		// Get scope types
		switch (scope_type) {
			case IVL_SCT_MODULE:
				scope_type_name      = "IVL_SCT_MODULE";
				scope_type_supported = true;
				break;
			case IVL_SCT_FUNCTION:
				scope_type_name = "IVL_SCT_FUNCTION";
				break;
			case IVL_SCT_TASK:
				scope_type_name      = "IVL_SCT_TASK";
				// scope_type_supported = true;
				break;
			case IVL_SCT_BEGIN:
				scope_type_name      = "IVL_SCT_BEGIN";
				// scope_type_supported = true;
				break;
			case IVL_SCT_FORK:
				scope_type_name = "IVL_SCT_FORK";
				break;
			case IVL_SCT_GENERATE:
				scope_type_name = "IVL_SCT_GENERATE";
				break;
			case IVL_SCT_PACKAGE:
				scope_type_name = "IVL_SCT_PACKAGE";
				break;
			case IVL_SCT_CLASS:
				scope_type_name = "IVL_SCT_CLASS";
				break;
			default:
				scope_type_name = "UNKNOWN";
				break;
		}	

		// Check if scope type supported 
		if (!scope_type_supported) {
			fprintf(stderr, "NOT-SUPPORTED: verilog scope type: %s", scope_type_name.c_str());
			fprintf(stderr, "File: %s Line: %d\n", ivl_scope_file(scopes[i]), ivl_scope_lineno(scopes[i]));
			exit(NOT_SUPPORTED_ERROR);
		}
	}
}

void Error::check_signal_exists_in_map(sig_map_t signals, ivl_signal_t sig){
	if (signals.count(sig) > 0) {
		fprintf(stderr, "ERROR: signal (%s) already exists in hashmap.\n", ivl_signal_name(sig));
		exit(DUPLICATE_SIGNALS_FOUND_ERROR);
	}
} 

void Error::check_signal_not_arrayed(ivl_signal_t signal){
	// Check if signal is arrayed
	if (ivl_signal_dimensions(signal) > 0) {
		fprintf(stderr, "NOT-SUPPORTED: arrayed signal (%s -- %d) encountered.\n", ivl_signal_name(signal), ivl_signal_dimensions(signal));
		exit(NOT_SUPPORTED_ERROR);
	} else {
		// Confirm that ARRAY_BASE is 0 (should be for non-arrayed signals)
		assert(ivl_signal_array_base(signal) == 0 && "NOT-SUPPORTED: non-arrayed signal with non-zero ARRAY_BASE.");

		// Confirm that ARRAY_COUNT is a (should be for non-arrayed signals)
		assert(ivl_signal_array_count(signal) == 1 && "NOT-SUPPORTED: non-arrayed signal with ARRAY_COUNT != 1.");
	}
}

void Error::check_logic_device_pins(ivl_net_logic_t logic_device, ivl_nexus_ptr_t logic_pin_nexus_ptr){
	// Only support input pins of LOGIC devices connected to SIGNALS
	if (ivl_nexus_ptr_sig(logic_pin_nexus_ptr) || ivl_nexus_ptr_con(logic_pin_nexus_ptr)) {
		return;
	} else if (ivl_nexus_ptr_log(logic_pin_nexus_ptr) != logic_device) {
		fprintf(stderr, "NOT-SUPPORTED: LOGIC device connected directly to LOGIC device.\n");
		exit(NOT_SUPPORTED_ERROR);
	} else if (ivl_nexus_ptr_lpm(logic_pin_nexus_ptr)) {
		fprintf(stderr, "NOT-SUPPORTED: LOGIC device connected directly to LPM device.\n");
		exit(NOT_SUPPORTED_ERROR);
	} else {
		fprintf(stderr, "NOT-SUPPORTED: LOGIC device connected UNKOWN nexus type.\n");
		exit(NOT_SUPPORTED_ERROR);
	}
}

void Error::unknown_nexus_type_error(ivl_nexus_ptr_t nexus_ptr){
	fprintf(stderr, "NOT-SUPPORTED: unkown nexus type for nexus.\n");
	exit(NOT_SUPPORTED_ERROR);
}
