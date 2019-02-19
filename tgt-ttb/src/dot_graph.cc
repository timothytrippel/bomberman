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

unsigned long DotGraph::get_msb(ivl_signal_t sig){
	unsigned num_dimens = ivl_signal_packed_dimensions(sig);

	if (num_dimens == 0) {
		return 0;
	} else{
		return ivl_signal_packed_msb(sig, 0);
	}
}

unsigned long DotGraph::get_lsb(ivl_signal_t sig){
	unsigned num_dimens = ivl_signal_packed_dimensions(sig);
	
	if (num_dimens == 0) {
		return 0;
	} else{
		return ivl_signal_packed_lsb(sig, 0);
	}
}

void DotGraph::init_graph(){
	open_file();
	fprintf(file_ptr_, "digraph G {\n");
}

string DotGraph::get_signal_fullname(ivl_signal_t sig){
	string scopename = string(ivl_scope_name(ivl_signal_scope(sig))); 
	string basename  = string(ivl_signal_basename(sig));
	string fullname  = string(scopename + string(".") + basename);

	return fullname;
}

string DotGraph::get_constant_fullname(ivl_net_const_t con){
	string scopename = string(ivl_scope_name(ivl_const_scope(con))); 
	string basename  = string(ivl_const_bits(con), (size_t)ivl_const_width(con));
	reverse(basename.begin(), basename.end());
	string fullname  = string(scopename + string(".const_") + basename);

	return fullname;
}

void DotGraph::add_signal_node(ivl_signal_t sig){
	string fullname = get_signal_fullname(sig);

	fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s[%lu:%lu]\"];\n", 
		fullname.c_str(), SIGNAL_NODE_SHAPE, fullname.c_str(), get_msb(sig), get_lsb(sig));
}

void DotGraph::add_local_signal_node(ivl_signal_t sig){
	string fullname = get_signal_fullname(sig);

	fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s[%lu:%lu]\"];\n", 
		fullname.c_str(), LOCAL_SIGNAL_NODE_SHAPE, fullname.c_str(), get_msb(sig), get_lsb(sig));
}

void DotGraph::add_ff_node(ivl_signal_t sig){
	string fullname = get_signal_fullname(sig);

	fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s[%lu:%lu]\"];\n", 
		fullname.c_str(), FF_NODE_SHAPE, fullname.c_str(), get_msb(sig), get_lsb(sig));
}

void DotGraph::add_input_node(ivl_signal_t sig){
	string fullname = get_signal_fullname(sig);

	fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s[%lu:%lu]\"];\n", 
		fullname.c_str(), INPUT_NODE_SHAPE, fullname.c_str(), get_msb(sig), get_lsb(sig));
}

void DotGraph::add_const_node(ivl_net_const_t con){
	string fullname = get_constant_fullname(con);

	fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s[%lu:%lu]\"]; /* Constant */\n", 
		fullname.c_str(), CONST_NODE_SHAPE, fullname.c_str(), (unsigned long) (ivl_const_width(con) - 1), (unsigned long) 0);
}

void DotGraph::add_connection(ivl_signal_t aff_sig, ivl_signal_t sig){
	// First Signal Name
	string fullname_1 = get_signal_fullname(aff_sig);
	
	// Second Signal Name
	string fullname_2 = get_signal_fullname(sig);

	// Add connection to .dot file
	fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"[%lu:%lu]->[%lu:%lu]\"];\n", 
		fullname_2.c_str(), fullname_1.c_str(), get_msb(sig), get_lsb(sig), get_msb(aff_sig), get_lsb(aff_sig));
}

void DotGraph::add_spliced_connection(ivl_signal_t aff_sig, 
									   unsigned long aff_sig_msb, 
									   unsigned long aff_sig_lsb, 
							           ivl_signal_t  sig, 
							           unsigned long sig_msb, 
							           unsigned long sig_lsb){
	// First Signal Name
	string fullname_1 = get_signal_fullname(aff_sig);
	
	// Second Signal Name
	string fullname_2 = get_signal_fullname(sig);

	// Add connection to .dot file
	fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"[%lu:%lu]->[%lu:%lu]\"];\n", 
		fullname_2.c_str(), fullname_1.c_str(), sig_msb, sig_lsb, aff_sig_msb, aff_sig_lsb);	
}

void DotGraph::add_const_connection(ivl_signal_t aff_sig, ivl_net_const_t con){
	// First Signal Name
	string fullname_1 = get_signal_fullname(aff_sig);
	
	// Second Signal Name
	string fullname_2 = get_constant_fullname(con);

	fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"[%lu:%lu]->[%lu:%lu]\"];\n", 
		fullname_2.c_str(), fullname_1.c_str(), 
		(unsigned long) (ivl_const_width(con) - 1), (unsigned long) 0, 
		get_msb(aff_sig), get_lsb(aff_sig));
}

void DotGraph::save_graph(){
	fprintf(file_ptr_, "}\n");
	close_file();
}

void DotGraph::save_graph(sig_map_t signals){
	// Create a signals map iterator
	sig_map_t::iterator signals_it = signals.begin();
 
	// Iterate over all signals in adjacency list
	while (signals_it != signals.end()) { 	
		// Get current signal/connections
		ivl_signal_t         signal = signals_it->first;
		vector<ivl_signal_t> conns  = signals_it->second;

		// Add node to graph
		if (ivl_signal_local(signal)){
			// signal was generated by IVL
			add_local_signal_node(signal);
		} else {
			// signal was defined in HDL
			add_signal_node(signal);
		}

		// Create a signals vector iterator
		vector<ivl_signal_t>::iterator conns_it = conns.begin();

		// Add connections to graph
		while (conns_it != conns.end()) {
			// Increment the iterator
			conns_it++;
		}

		// Increment the iterator
		signals_it++;
	}

	save_graph();
}
