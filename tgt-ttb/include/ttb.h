/*
File:        ttb.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_HEADER__
#define __TTB_HEADER__

// Standard Headers
#include <string>

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "signal_graph.h"

using namespace std;

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
string       get_signal_fullname(ivl_signal_t signal);
string       get_constant_fullname(ivl_net_const_t constant);
unsigned int get_signal_msb(ivl_signal_t signal);
unsigned int get_signal_lsb(ivl_signal_t signal);
unsigned int get_const_msb(ivl_net_const_t constant);

// ----------------------------------------------------------------------------------
// ------------------------ Dot Graph Helper Functions ------------------------------
// ----------------------------------------------------------------------------------
string get_signal_node_label(ivl_signal_t signal);
string get_const_node_label(ivl_net_const_t constant);

string get_signal_connection_label(ivl_signal_t source_signal, 
                                   ivl_signal_t sink_signal);

string get_sliced_signal_connection_label(ivl_signal_t source_signal, 
                                          ivl_signal_t sink_signal, 
                                          SliceInfo    signal_slice);

string get_const_connection_label(ivl_net_const_t source_constant, 
                                  ivl_signal_t    sink_signal);

// ----------------------------------------------------------------------------------
// --------------------- Combinational Logic Processing -----------------------------
// ----------------------------------------------------------------------------------
void find_combinational_connections();

// Nexus
void propagate_nexus(ivl_nexus_t  nexus,
                     ivl_signal_t sink_signal,
                     SignalGraph* sg,
                     string       ws);

// Logic
void propagate_logic(ivl_net_logic_t logic,
                     ivl_nexus_t     sink_nexus,
                     ivl_signal_t    sink_signal,
                     SignalGraph*    sg,
                     string          ws);

const char* get_logic_type_as_string(ivl_net_logic_t logic);

// LPM
void propagate_lpm(ivl_lpm_t    lpm,
                   ivl_nexus_t  sink_nexus,
                   ivl_signal_t sink_signal,
                   SignalGraph* sg,
                   string       ws);

void process_lpm_part_select(ivl_lpm_t    lpm, 
                             ivl_signal_t sink_signal,
                             SignalGraph* sg,
                             string       ws);

void process_lpm_concat(ivl_lpm_t    lpm,
                        ivl_signal_t sink_signal,
                        SignalGraph* sg,
                        string       ws);

void process_lpm_mux(ivl_lpm_t    lpm,
                     ivl_signal_t sink_signal,
                     SignalGraph* sg,
                     string       ws);

void process_lpm_basic(ivl_lpm_t    lpm,
                       ivl_signal_t sink_signal,
                       SignalGraph* sg,
                       string       ws);

const char* get_lpm_type_as_string(ivl_lpm_t lpm);

// Constant
void propagate_constant(ivl_net_const_t constant,
                        ivl_signal_t sink_signal, 
                        SignalGraph* sg,
                        string       ws);

const char* get_const_type_as_string(ivl_net_const_t constant);


// ----------------------------------------------------------------------------------
// ------------------------ Behavioral Logic Processing -----------------------------
// ----------------------------------------------------------------------------------
void find_behavioral_connections(ivl_design_t design, SignalGraph* sg);

// Process
int process_process(ivl_process_t process, void* data);
const char* get_process_type_as_string(ivl_process_t process);

// Expression
void process_expression(ivl_expr_t expression, SignalGraph* sg, string ws);
void process_expression_signal(ivl_expr_t expression, SignalGraph* sg, string ws);
const char* get_expr_type_as_string(ivl_expr_t expression);

// Statement
void process_statement(ivl_statement_t statement, 
                              SignalGraph* sg, 
                              string ws);

void process_statement_wait(ivl_statement_t statement, SignalGraph* sg, string ws);
void process_statement_condit(ivl_statement_t statement, SignalGraph* sg, string ws);
const char* get_statement_type_as_string(ivl_statement_t statement);

#endif
