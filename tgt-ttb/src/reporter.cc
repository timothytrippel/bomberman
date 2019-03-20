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
#include "ttb.h"
#include "signal.h"
#include "signal_graph.h"
#include "reporter.h"

// ------------------------------------------------------------
// ----------------------- Constructors -----------------------
// ------------------------------------------------------------

Reporter::Reporter(): 
    file_path_(NULL), 
    file_ptr_(NULL) {

    // Initialize file to print to and times queue
    init();
}

Reporter::Reporter(FILE* file_ptr): 
    file_path_(NULL), 
    file_ptr_(file_ptr) {

    // Initialize file to print to and times queue
    init();
}

Reporter::Reporter(const char* p) {
    file_ptr_ = NULL;
    set_file_path(p);

    // Initialize file to print to and times queue
    init();
}

// ------------------------------------------------------------
// ----------------------- Destructors ------------------------
// ------------------------------------------------------------

Reporter::~Reporter() {
    
    fprintf(DESTRUCTOR_PRINTS_FILE_PTR, "Executing Reporter destructor...\n");

    // Close file if its open and not STDOUT
    if (file_ptr_ && (file_ptr_ != stdout)) {
        close_file();
    }

    // Delete the start times queue
    if (start_times_q_) {
        delete(start_times_q_);
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
// ---------------------- Task Tracking -----------------------
// ------------------------------------------------------------

void Reporter::start_task(const char* message) {
    // Print starting message
    print_message(message);

    // Check that not more than two timers on the queue
    assert(start_times_q_->size() < 2 && 
        "ERROR: cannot track more concurrent task times.\n");

    // Start timer and push to queue
    start_times_q_->push_back(clock());
}

void Reporter::end_task() {
    double start_time     = 0;
    double execution_time = 0;

    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Get start time
    start_time = start_times_q_->back();
    start_times_q_->pop_back();

    // Compute execution time
    execution_time = (clock() - start_time) / (double) CLOCKS_PER_SEC;

    // Print execution time
    fprintf(file_ptr_, "\nExecution Time: %f (s)\n", execution_time);
}

// ------------------------------------------------------------
// --------------------- Message Printing ---------------------
// ------------------------------------------------------------

void Reporter::print_message(const char* message) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print init message
    line_separator();
    fprintf(file_ptr_, "%s\n\n", message);
}

void Reporter::line_separator() const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print line separator
    fprintf(file_ptr_, "%s\n", LINE_SEPARATOR);
}

void Reporter::configurations(cmd_args_map_t* cmd_args) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print output filename
    fprintf(file_ptr_, "Output DOT graph filname: %s\n", 
        cmd_args->at(OUTPUT_FILENAME_FLAG).c_str());

    // Print CLK basename
    fprintf(file_ptr_, "Clock signal basename:    %s\n", 
        cmd_args->at(CLK_BASENAME_FLAG).c_str());
}

void Reporter::root_scopes(ivl_scope_t* scopes, unsigned int num_scopes) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print number of scopes
    fprintf(file_ptr_, "Found %d top-level module(s):\n", num_scopes);
    
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
    fprintf(file_ptr_, "Number of signals found: %lu\n", num_signals);
}

void Reporter::signal_names(sig_map_t signals_map) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print name of all signals in vector
    fprintf(file_ptr_, "Signal Names:\n");      

    // Create iterator
    sig_map_t::iterator it = signals_map.begin();
 
    // Iterate over the map until the end
    while (it != signals_map.end()) {   
        // Print signal name
        fprintf(file_ptr_, "    %s\n", it->second->get_fullname().c_str());
 
        // Increment the iterator
        it++;
    }
}

void Reporter::graph_stats(SignalGraph* sg) const {
    // Check that file has been opened for writing report
    assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

    // Print number of signals enumerated in design
    fprintf(file_ptr_, "Number of signals found:       %lu\n", sg->get_num_signals());

    // Print number of local signals processed (removed) in design
    fprintf(file_ptr_, "Number of local signals found: %lu\n", sg->get_num_local_signals());

    // Print number of constants enumerated in design
    fprintf(file_ptr_, "Number of constants found:     %lu\n", sg->get_num_constants());
    
    // Print number of connections enumerated in design
    fprintf(file_ptr_, "Number of connections found:   %lu\n", sg->get_num_connections());
}

// ------------------------------------------------------------
// -------------------------- Other ---------------------------
// ------------------------------------------------------------

void Reporter::init() {
    // Open output file or print to STDOUT
    if (!file_path_ && !file_ptr_){
        file_ptr_ = stdout;
    } else if (!file_ptr_) {
        open_file();
    }

    // Create start times queue
    start_times_q_ = new times_q_t();
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
