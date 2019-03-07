/*
File:        dot_graph.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <cassert>
#include <string>

// TTB Headers
#include "dot_graph.h"

DotGraph::DotGraph(): file_path_(NULL), file_ptr_(NULL){}

DotGraph::DotGraph(const char* p): file_ptr_(NULL){
    set_file_path(p);
}

void DotGraph::set_file_path(const char* p){
    file_path_ = p;
}

const char* DotGraph::get_file_path(){
    return file_path_;
}

FILE* DotGraph::get_file_ptr(){
    return file_ptr_;
}

void DotGraph::open_file(){
    file_ptr_ = fopen(file_path_, "w");
    if (!file_ptr_) {
        printf("ERROR: Could not open file %s\n", file_path_ ? file_path_ : "stdout");
        exit(-4);
    }
}

void DotGraph::close_file(){
    fclose(file_ptr_);
    file_ptr_ = NULL;
}

void DotGraph::init_graph(){
    open_file();
    fprintf(file_ptr_, "digraph G {\n");
}

void DotGraph::add_node(string name, string label, string shape){
    fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s%s\"];\n", 
        name.c_str(), shape.c_str(), name.c_str(), label.c_str());
}

void DotGraph::add_connection(string source_node_name,
                              string sink_node_name, 
                              string connection_label,
                              string ws){

    // Debug Print
    fprintf(stdout, "%sADDING CONNECTION from %s to %s\n", 
        ws.c_str(), 
        source_node_name.c_str(), 
        sink_node_name.c_str());

    // Add connection to .dot file
    fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"%s\"];\n", 
        source_node_name.c_str(), sink_node_name.c_str(), connection_label.c_str());    
}

void DotGraph::save_graph(){
    fprintf(file_ptr_, "}\n");
    close_file();
}

// void DotGraph::save_graph(sig_map_t signals){
//  // Create a signals map iterator
//  sig_map_t::iterator signals_it = signals.begin();
 
//  // Iterate over all signals in adjacency list
//  while (signals_it != signals.end()) {   
//      // Get current signal/connections
//      ivl_signal_t         signal = signals_it->first;
//      vector<ivl_signal_t> conns  = signals_it->second;

//      // Add node to graph
//      if (ivl_signal_local(signal)){
//          // signal was generated by IVL
//          add_local_signal_node(signal);
//      } else {
//          // signal was defined in HDL
//          add_signal_node(signal);
//      }

//      // Create a signals vector iterator
//      vector<ivl_signal_t>::iterator conns_it = conns.begin();

//      // Add connections to graph
//      while (conns_it != conns.end()) {
//          // Increment the iterator
//          conns_it++;
//      }

//      // Increment the iterator
//      signals_it++;
//  }

//  save_graph();
// }
