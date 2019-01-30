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

// Error Codes
typedef enum ttb_error_type_e {
	NO_ERROR         = 0,
	FILE_ERROR       = 1,
	SCOPE_TYPE_ERROR = 2,
} ttb_error_type_t;

class Error {
	public:
		Error();
		void check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes);
};

#endif
