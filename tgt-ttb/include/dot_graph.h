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

// Standard Headers
#include <cstdio>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"

// Dot Graph Shapes
#define SIGNAL_NODE_SHAPE       "ellipse"
#define LOCAL_SIGNAL_NODE_SHAPE "circle"
#define FF_NODE_SHAPE           "square"
#define INPUT_NODE_SHAPE        "none"
#define CONST_NODE_SHAPE        "rectangle"

class DotGraph {
    public:
        DotGraph();
        DotGraph(const char* p);
        void        set_file_path(const char* p);
        const char* get_file_path();
        void        init_graph();
        void        add_node(string name, string label, string shape);
        void        add_connection(string source_node_name,
                                   string sink_node_name, 
                                   string connection_label,
                                   string ws);
        void        save_graph();
        // void         save_graph(sig_map_t signals);
    private:
        const char*   file_path_;
        FILE*         file_ptr_;
        FILE*         get_file_ptr();
        void          open_file();
        void          close_file();
};

#endif