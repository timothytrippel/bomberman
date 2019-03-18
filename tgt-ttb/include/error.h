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
#include "signal.h"
#include "connection.h"

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
    STRUCTURAL_CONNECTIONS_ERROR  = 4,
    BEHAVIORAL_CONNECTIONS_ERROR  = 5,
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
        static void check_signal_not_multidimensional(sig_map_t signals, ivl_signal_t signal);
        static void check_event_nexus(ivl_nexus_t nexus, ivl_statement_t statement);
        static void check_lvals_not_concatenated(unsigned int num_lvals, ivl_statement_t statement);
        static void check_lval_not_nested(ivl_lval_t lval, ivl_statement_t statement);
        static void check_lval_not_memory(ivl_lval_t lval, ivl_statement_t statement);
        static void check_part_select_expr(ivl_obj_type_t obj_type, ivl_statement_t statement);
        static void check_slice_tracking_stack(unsigned int size, unsigned int size_limit);

        // Error Reporting Functions
        // Unknown Types
        static void unknown_ivl_obj_type(ivl_obj_type_t obj_type);
        static void unknown_nexus_type();
        static void unknown_signal_port_type(ivl_signal_port_t port_type);
        static void unknown_part_select_lpm_type(ivl_lpm_type_t lpm_type);
        static void unknown_statement_type(ivl_statement_type_t statement_type);
        static void unknown_expression_type(ivl_expr_type_t expression_type);
        // Other
        static void not_supported(const char* message);
        static void null_ivl_obj_type();
        static void connecting_signal_not_in_graph(sig_map_t signals, ivl_signal_t source_signal);
        static void processing_behavioral_connections();
        static void non_local_signal_connection();
};

#endif
