/*
File:        reporter.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __REPORTER_HEADER__
#define __REPORTER_HEADER__

// Standard Headers
#include <vector>

using namespace std;

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"

class Reporter {
	public:
		Reporter();
		Reporter(const char* p);
		void 		set_file_path(const char* p);
		const char* get_file_path();
		void 		init(const char* init_message);
		void 		print_message(const char* message);
		void 		root_scopes(ivl_scope_t* scopes, unsigned int num_scopes);
		void 		num_signals(sig_map_t signals);
		void 		signal_names(sig_map_t signals);
		void 		end();
		
	private:
		const char* file_path_;
		FILE* 		file_ptr_;
		clock_t 	start_time_;
		double 	    execution_time_;
		FILE* 		get_file_ptr();
		void 		open_file();
		void 		close_file();
};

#endif
