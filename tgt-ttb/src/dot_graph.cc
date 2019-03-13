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
#include "connection.h"
#include "dot_graph.h"

// ------------------------------------------------------------
// ----------------------- Constructors -----------------------
// ------------------------------------------------------------

DotGraph::DotGraph(): file_path_(NULL), file_ptr_(NULL) {}

DotGraph::DotGraph(const char* p): file_ptr_(NULL) {
    set_file_path(p);
}

// ------------------------------------------------------------
// ------------------------- Getters --------------------------
// ------------------------------------------------------------

const char* DotGraph::get_file_path() const {
    return file_path_;
}

// ------------------------------------------------------------
// ------------------------- Setters --------------------------
// ------------------------------------------------------------

void DotGraph::set_file_path(const char* p) {
    file_path_ = p;
}

// ------------------------------------------------------------
// -------------------- Graph Construction --------------------
// ------------------------------------------------------------

void DotGraph::init_graph() {
    open_file();
    fprintf(file_ptr_, "digraph G {\n");
}

void DotGraph::add_node(Signal signal, string ws) const {
    // Debug Print
    fprintf(stdout, "%sADDING NODE (%s)\n", 
        ws.c_str(), 
        signal.get_fullname().c_str());

    fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s%s\"];\n", 
        signal.get_fullname().c_str(), 
        signal.get_dot_shape().c_str(), 
        signal.get_fullname().c_str(), 
        signal.get_dot_label().c_str());
}

void DotGraph::add_connection(Connection conn, string ws) const {

    // Debug Print
    fprintf(stdout, "%sADDING CONNECTION from %s to %s\n", 
        ws.c_str(), 
        conn.get_source().get_fullname().c_str(), 
        conn.get_sink().get_fullname().c_str());

    // Add connection to .dot file
    fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"%s\"];\n", 
        conn.get_source().get_fullname().c_str(), 
        conn.get_sink().get_fullname().c_str(), 
        conn.get_dot_label().c_str());    
}

void DotGraph::save_graph() {
    fprintf(file_ptr_, "}\n");
    close_file();
}

// ------------------------------------------------------------
// --------------------- File Operations ----------------------
// ------------------------------------------------------------

void DotGraph::open_file() {
    file_ptr_ = fopen(file_path_, "w");
    if (!file_ptr_) {
        printf("ERROR: Could not open file %s\n", file_path_ ? file_path_ : "stdout");
        exit(-4);
    }
}

void DotGraph::close_file() {
    fclose(file_ptr_);
    file_ptr_ = NULL;
}
