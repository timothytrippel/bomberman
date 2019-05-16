/*
File:        error.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __ERROR_HEADER__
#define __ERROR_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <vector>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include <signal.h>
#include <connection.h>

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------ Error Codes -----------------------
// ------------------------------------------------------------

typedef enum ttb_error_type_e {
    NO_ERROR                      = 0,
    NOT_SUPPORTED_ERROR           = 1,
    FILE_ERROR                    = 2,  
    DUPLICATE_SIGNALS_FOUND_ERROR = 3,
    CONCURRENT_CONNECTIONS_ERROR  = 4,
    PROCEDURAL_CONNECTIONS_ERROR  = 5,
    SLICE_TRACKING_ERROR          = 6,
} ttb_error_type_t;

// ------------------------------------------------------------
// -------------------------- Error ---------------------------
// ------------------------------------------------------------

class Error {
    public:
        // Data Validation Functions
        static void check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes);
        static void check_signal_exists_in_map(sig_map_t signals, ivl_signal_t signal);
        static void check_signal_not_arrayed(sig_map_t signals, ivl_signal_t signal);
        static void check_arrayed_signal(sig_map_t signals, ivl_signal_t signal);
        static void check_signal_not_multidimensional(sig_map_t signals, ivl_signal_t signal);
        static void check_lvals_not_concatenated(unsigned int num_lvals, ivl_statement_t statement);
        static void check_lval_not_nested(ivl_lval_t lval, ivl_statement_t statement);
        static void check_part_select_expr(ivl_obj_type_t obj_type, ivl_statement_t statement);

        // Error Reporting Functions
        // Unknown Types
        static void unknown_ivl_obj_type(ivl_obj_type_t obj_type);
        static void unknown_nexus_type();
        static void unknown_signal_port_type(ivl_signal_port_t port_type);
        static void unknown_part_select_lpm_type(ivl_lpm_type_t lpm_type);
        static void unknown_statement_type(ivl_statement_type_t statement_type);
        static void unknown_expression_type(ivl_expr_type_t expression_type);
        // Warnings
        static void constant_event_nexus_ptr_warning(ivl_statement_t stmt);
        static void stask_statement_type_warning(ivl_statement_t stmt);
        static void utask_statement_type_warning(ivl_statement_t stmt);
        static void while_statement_type_warning(ivl_statement_t stmt);
        static void repeat_statement_type_warning(ivl_statement_t stmt);
        static void unkown_event_source_signal_warning(ivl_statement_t stmt);
        // Other
        static void not_supported(const char* message);
        static void null_ivl_obj_type();
        static void connecting_signal_not_in_graph(sig_map_t signals, ivl_signal_t source_signal);
        static void popping_source_signals_queue(unsigned int num_signals, unsigned int queue_size);
        static void processing_procedural_connections();
        static void non_local_signal_connection();
        static void multiple_valid_event_nexus_ptrs(ivl_statement_t stmt);
        static void zero_event_nexus_ptrs(ivl_statement_t stmt);
        static void constant_event_nexus_ptr(ivl_statement_t stmt);
};

#endif
