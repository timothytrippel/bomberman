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

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <string>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "signal.h"
#include "signal_graph.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------- Defines --------------------------
// ------------------------------------------------------------

#define IGNORE_FILEPATH_FLAG  "ignore_filepath"
#define IGNORE_CONSTANTS_FLAG "ignore_consts"

// ------------------------------------------------------------
// -------------- CMD Line Arguments Processing ---------------
// ------------------------------------------------------------

cmd_args_map_t* process_cmd_line_args(ivl_design_t des);

// ------------------------------------------------------------
// ----------------- Structural HDL Processing ----------------
// ------------------------------------------------------------

void find_structural_connections();

// Nexus
void propagate_nexus(ivl_nexus_t  nexus,
                     Signal*      sink_signal,
                     SignalGraph* sg,
                     string       ws);

// Logic
void propagate_logic(ivl_net_logic_t logic,
                     ivl_nexus_t     sink_nexus,
                     Signal*         sink_signal,
                     SignalGraph*    sg,
                     string          ws);

// LPM
void propagate_lpm(ivl_lpm_t    lpm,
                   ivl_nexus_t  sink_nexus,
                   Signal*      sink_signal,
                   SignalGraph* sg,
                   string       ws);

void process_lpm_basic(ivl_lpm_t    lpm,
                       Signal*      sink_signal,
                       SignalGraph* sg,
                       string       ws);

void process_lpm_part_select(ivl_lpm_t    lpm, 
                             Signal*      sink_signal,
                             SignalGraph* sg,
                             string       ws);

void process_lpm_concat(ivl_lpm_t    lpm,
                        Signal*      sink_signal,
                        SignalGraph* sg,
                        string       ws);

void process_lpm_mux(ivl_lpm_t    lpm,
                     Signal*      sink_signal,
                     SignalGraph* sg,
                     string       ws);

// Constant
void propagate_constant(ivl_net_const_t constant,
                        Signal*         sink_signal, 
                        SignalGraph*    sg,
                        string          ws);

// ------------------------------------------------------------
// ----------------- Behavioral HDL Processing ----------------
// ------------------------------------------------------------

void find_behavioral_connections(ivl_design_t design, 
                                 SignalGraph* sg);

// Process
int process_process(ivl_process_t process, void* data);

// Expression
unsigned int process_expression(ivl_expr_t   expression, 
                                SignalGraph* sg, 
                                string       ws);

unsigned int process_expression_signal(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws);

unsigned int process_expression_number(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws);

unsigned int process_expression_concat(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws);

unsigned int process_expression_unary(ivl_expr_t   expression, 
                                      SignalGraph* sg, 
                                      string       ws);

unsigned int process_expression_binary(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws);

unsigned int process_expression_ternary(ivl_expr_t   expression, 
                                        SignalGraph* sg, 
                                        string       ws);

// Event
unsigned int process_event(ivl_event_t     event, 
                           ivl_statement_t statement, 
                           SignalGraph*    sg, 
                           string          ws);

unsigned int process_event_nexus(ivl_nexus_t     nexus, 
                                 ivl_statement_t statement, 
                                 SignalGraph*    sg, 
                                 string          ws);

// Statement
void process_statement(ivl_statement_t statement, 
                       SignalGraph*    sg, 
                       string          ws);

void process_statement_wait(ivl_statement_t statement, 
                            SignalGraph*    sg, 
                            string          ws);

void process_statement_condit(ivl_statement_t statement, 
                              SignalGraph*    sg, 
                              string          ws);

unsigned int process_statement_assign_partselect(Signal*         offset, 
                                                 ivl_statement_t statement);

void process_statement_assign(ivl_statement_t statement, 
                              SignalGraph*    sg, 
                              string          ws);

void process_statement_block(ivl_statement_t statement, 
                             SignalGraph*    sg, 
                             string          ws);

void process_statement_case(ivl_statement_t statement, 
                            SignalGraph*    sg, 
                            string          ws);

void process_statement_delay(ivl_statement_t statement, 
                             SignalGraph*    sg, 
                             string          ws);

// ------------------------------------------------------------
// -------------------------- Other ---------------------------
// ------------------------------------------------------------

const char* get_logic_type_as_string(ivl_net_logic_t logic);
const char* get_lpm_type_as_string(ivl_lpm_t lpm);
const char* get_const_type_as_string(ivl_net_const_t constant);
const char* get_process_type_as_string(ivl_process_t process);
const char* get_expr_type_as_string(ivl_expr_t expression);
const char* get_statement_type_as_string(ivl_statement_t statement);

#endif
