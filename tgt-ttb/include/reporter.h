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

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <cstdio>
#include <vector>

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "signal.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------ Reporter --------------------------
// ------------------------------------------------------------

class Reporter {
    public:
        // Constructors
        Reporter();
        Reporter(const char* p);
        ~Reporter();
        
        // Getters
        const char* get_file_path() const;

        // Setters
        void set_file_path(const char* p);

        // Message Printing
        void print_message(const char* message) const;
        void root_scopes(ivl_scope_t* scopes, unsigned int num_scopes) const;
        void num_signals(unsigned long num_signals) const;
        void graph_stats(SignalGraph* sg) const;
        void signal_names(sig_map_t signals_map) const;
        void line_separator() const;

        // Other
        void init(const char* init_message);
        void end();
    
    private:
        const char* file_path_;
        FILE*       file_ptr_;
        clock_t     start_time_;
        double      execution_time_;

        // File Operations
        void open_file();
        void close_file();
};

#endif
