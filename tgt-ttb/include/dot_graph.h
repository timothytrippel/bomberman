/*
File:        dot_graph.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __DOT_GRAPH_HEADER__
#define __DOT_GRAPH_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <cstdio>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "signal.h"
#include "connection.h"

// ------------------------------------------------------------
// ------------------------- Dot Graph ------------------------
// ------------------------------------------------------------

class DotGraph {
    public:
        // Constructors
        DotGraph();
        DotGraph(string p);

        // Destructors
        ~DotGraph();
        
        // Getters
        string get_file_path() const;

        // Setters
        void set_file_path(string p);

        // Graph Construction
        void init_graph();
        void add_node(Signal* signal, string ws) const;
        void add_connection(Connection* conn, string ws) const;
        void save_graph();

    private:
        string file_path_;
        FILE*  file_ptr_;

        // File Operations
        void open_file();
        void close_file();
};

#endif
