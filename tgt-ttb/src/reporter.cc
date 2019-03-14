/*
File:        reporter.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <cassert>
#include <ctime>
#include <cstring>
#include <string>

// TTB Headers
#include "ttb_typedefs.h"
#include "signal.h"
#include "signal_graph.h"
#include "reporter.h"

// ------------------------------------------------------------
// ----------------------- Constructors -----------------------
// ------------------------------------------------------------

Reporter::Reporter(): 
    file_path_(NULL), 
    file_ptr_(NULL) {}

Reporter::Reporter(const char* p) {
    file_ptr_ = NULL;
    set_file_path(p);
}

Reporter::~Reporter() {
    // Close file if its open and not STDOUT
    if (file_ptr_) {
        close_file();
    }
}

// ------------------------------------------------------------
// ------------------------- Getters --------------------------
// ------------------------------------------------------------

const char* Reporter::get_file_path() const {
    return file_path_;
}

// ------------------------------------------------------------
// ------------------------- Setters --------------------------
// ------------------------------------------------------------

void Reporter::set_file_path(const char* p) {
    file_path_ = p;
}

// ------------------------------------------------------------
// --------------------- Message Printing ---------------------
// ------------------------------------------------------------

void Reporter::print_message(const char* message) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print init message
    line_separator();
    fprintf(file_ptr_, "%s\n", message);
}

void Reporter::root_scopes(ivl_scope_t* scopes, unsigned int num_scopes) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print number of scopes
    fprintf(file_ptr_, "\nFound %d top-level module(s):\n", num_scopes);
    
    string scope_name = "UNKONWN";

    for (unsigned int i = 0; i < num_scopes; i++){  
        // Get scope name
        scope_name = ivl_scope_name(scopes[i]);

        // Print scope name
        fprintf(file_ptr_, "    %s\n", scope_name.c_str());
    }
}

void Reporter::num_signals(unsigned long num_signals) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print number of signals enumerated in design
    fprintf(file_ptr_, "\nNumber of signals found: %lu\n", num_signals);
}

void Reporter::graph_stats(SignalGraph* sg) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print number of signals enumerated in design
    fprintf(file_ptr_, "\nNumber of signals found:     %lu\n", sg->get_num_signals());

    // Print number of constants enumerated in design
    fprintf(file_ptr_, "Number of constants found:   %lu\n", sg->get_num_constants());
    
    // Print number of connections enumerated in design
    fprintf(file_ptr_, "Number of connections found: %lu\n", sg->get_num_connections());
}

void Reporter::signal_names(sig_map_t signals_map) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print name of all signals in vector
    fprintf(file_ptr_, "\nSignal Names:\n");      

    // Create a signals map iterator
    sig_map_t::iterator it = signals_map.begin();
 
    // Iterate over the map using Iterator till end.
    while (it != signals_map.end()) {   
        // Print signal name
        fprintf(file_ptr_, "    %s\n", it->second->get_fullname().c_str());
 
        // Increment the iterator
        it++;
    }
}

void Reporter::line_separator() const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print line separator
    fprintf(file_ptr_, "\n%s\n", LINE_SEPARATOR);
}

// ------------------------------------------------------------
// -------------------------- Other ---------------------------
// ------------------------------------------------------------

void Reporter::init(const char* init_message) {
    // Open output file or print to STDOUT
    if (file_path_ == NULL){
        file_ptr_ = stdout;
    } else {
        open_file();
    }

    print_message(init_message);

    // Record start time
    start_time_ = clock();
}

void Reporter::end(){
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Record current execution time
    execution_time_ = (clock() - start_time_) / (double) CLOCKS_PER_SEC;
    fprintf(file_ptr_, "\nExecution Time: %f (s)\n", execution_time_);
    fprintf(file_ptr_, "%s\n", LINE_SEPARATOR);

    // Close output file
    fclose(file_ptr_);
}

void Reporter::open_file(){
    file_ptr_ = fopen(file_path_, "w");
    if (!file_ptr_) {
       fprintf(stderr, "ERROR: Could not open file %s\n", 
            file_path_ ? file_path_ : "stdout");
        exit(-1);
    }
}

void Reporter::close_file(){
    if (file_ptr_ != stdout){
        fclose(file_ptr_);
    }
    file_ptr_ = NULL;
}
