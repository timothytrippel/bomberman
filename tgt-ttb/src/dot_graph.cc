/*
File:        dot_graph.cc
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
#include <string>

// TTB Headers
#include "ttb_typedefs.h"
#include "connection.h"
#include "dot_graph.h"
#include "error.h"

// ------------------------------------------------------------
// ----------------------- Constructors -----------------------
// ------------------------------------------------------------

DotGraph::DotGraph(): file_path_(NULL), file_ptr_(NULL) {}

DotGraph::DotGraph(string p): file_ptr_(NULL) {
    set_file_path(p);
}

// ------------------------------------------------------------
// ----------------------- Destructors ------------------------
// ------------------------------------------------------------

DotGraph::~DotGraph() {

    fprintf(DESTRUCTOR_PRINTS_FILE_PTR, "Executing DotGraph destructor...\n");

    // Close file if its open and not STDOUT
    if (file_ptr_ && (file_ptr_ != stdout)) {
        close_file();
    }
}

// ------------------------------------------------------------
// ------------------------- Getters --------------------------
// ------------------------------------------------------------

string DotGraph::get_file_path() const {
    return file_path_;
}

// ------------------------------------------------------------
// ------------------------- Setters --------------------------
// ------------------------------------------------------------

void DotGraph::set_file_path(string p) {
    file_path_ = p;
}

// ------------------------------------------------------------
// -------------------- Graph Construction --------------------
// ------------------------------------------------------------

void DotGraph::init_graph() {
    open_file();
    fprintf(file_ptr_, "digraph G {\n");
}

void DotGraph::add_node(Signal* signal, string ws) const {
    if (file_ptr_) {   

        // Debug Print
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sADDING NODE (%s)\n", 
            ws.c_str(), 
            signal->get_fullname().c_str());

        // Print to dot file
        fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s%s\"];\n", 
            signal->get_fullname().c_str(), 
            signal->get_dot_shape().c_str(), 
            signal->get_fullname().c_str(), 
            signal->get_dot_label().c_str());

    } else {

        fprintf(stderr, "ERROR: dot graph file (%s) not open.\n", 
            !file_path_.empty() ? file_path_.c_str() : "stdout");
        
        exit(FILE_ERROR);
    }
}

void DotGraph::add_connection(Connection* conn, string ws) const {
    if (file_ptr_) {

        // Debug Print
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sADDING CONNECTION: %s --> %s\n", 
            ws.c_str(), 
            conn->get_source()->get_fullname().c_str(), 
            conn->get_sink()->get_fullname().c_str());

        // Add connection to .dot file
        fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"%s\"];\n", 
            conn->get_source()->get_fullname().c_str(), 
            conn->get_sink()->get_fullname().c_str(), 
            conn->get_dot_label().c_str());

    } else {

        fprintf(stderr, "ERROR: dot graph file (%s) not open.\n", 
            !file_path_.empty() ? file_path_.c_str() : "stdout");

        exit(FILE_ERROR);
    } 
}

void DotGraph::save_graph() {
    // Check if file is still open
    if (file_ptr_) {
        fprintf(file_ptr_, "}\n");
        close_file();
    }
}

// ------------------------------------------------------------
// --------------------- File Operations ----------------------
// ------------------------------------------------------------

void DotGraph::open_file() {
    file_ptr_ = fopen(file_path_.c_str(), "w");
    if (!file_ptr_) {
        fprintf(stderr, "ERROR: Could not open file %s\n", 
            !file_path_.empty() ? file_path_.c_str() : "stdout");
        exit(FILE_ERROR);
    }
}

void DotGraph::close_file() {
    fclose(file_ptr_);
    file_ptr_ = NULL;
}
