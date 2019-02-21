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
// #include <cstdlib>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"

// Dot Graph Shapes
#define SIGNAL_NODE_SHAPE       "ellipse"
#define LOCAL_SIGNAL_NODE_SHAPE "circle"
#define FF_NODE_SHAPE           "square"
#define INPUT_NODE_SHAPE        "none"
#define CONST_NODE_SHAPE        "ellipse"

class DotGraph {
	public:
		DotGraph();
		DotGraph(const char* p);
		void 		set_file_path(const char* p);
		const char* get_file_path();
		void 		init_graph();
		void 		add_signal_node(ivl_signal_t sig);
		void 		add_local_signal_node(ivl_signal_t sig);
		void 		add_ff_node(ivl_signal_t sig);
		void 		add_input_node(ivl_signal_t sig);
		void 		add_const_node(ivl_net_const_t con);
		void 		add_connection(ivl_signal_t aff_sig, ivl_signal_t sig);
		void 		add_sliced_connection(ivl_signal_t  aff_sig, 
                                          unsigned long aff_sig_msb, 
                                          unsigned long aff_sig_lsb,
                                          ivl_signal_t  sig, 
                                          unsigned long sig_msb, 
                                          unsigned long sig_lsb);
		void 		add_const_connection(ivl_signal_t  aff_sig, ivl_net_const_t con);
		void 		save_graph();
		void 		save_graph(sig_map_t signals);
	private:
		const char*   file_path_;
		FILE* 		  file_ptr_;
		FILE* 		  get_file_ptr();
		string 		  get_signal_fullname(ivl_signal_t sig);
		string 		  get_constant_fullname(ivl_net_const_t con);
		unsigned long get_msb(ivl_signal_t sig);
		unsigned long get_lsb(ivl_signal_t sig);
		void 		  open_file();
		void 		  close_file();
};

#endif