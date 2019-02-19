/*
File:        error.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __ERROR_HEADER__
#define __ERROR_HEADER__

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"

// Error Codes
typedef enum ttb_error_type_e {
	NO_ERROR                      = 0,
	NOT_SUPPORTED_ERROR           = 1,
	FILE_ERROR                    = 2,	
	DUPLICATE_SIGNALS_FOUND_ERROR = 3,
} ttb_error_type_t;

class Error {
	public:
		Error();
		// Data Validation Functions
		static void check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes);
		static void check_signal_exists_in_map(sig_map_t signals, ivl_signal_t sig);
		static void check_signal_not_arrayed(ivl_signal_t signal);
		static void check_logic_device_pins(ivl_net_logic_t logic_device, ivl_nexus_ptr_t logic_pin_nexus_ptr);

		// Error Reporting Functions
		static void unknown_nexus_type_error(ivl_nexus_ptr_t nexus);
};

#endif
