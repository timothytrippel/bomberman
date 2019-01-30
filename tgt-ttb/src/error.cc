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
				scope_type_name = "IVL_SCT_TASK";
				break;
			case IVL_SCT_BEGIN:
				scope_type_name = "IVL_SCT_BEGIN";
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
			fprintf(stderr, "UNSUPPORTED: verilog scope type: %s", scope_type_name.c_str());
			exit(SCOPE_TYPE_ERROR);
		}
	}
}
