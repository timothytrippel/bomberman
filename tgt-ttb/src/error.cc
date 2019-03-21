/*
File:        error.cc
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
#include <cstdio>
#include <cassert>
#include <cstring>
#include <string>

// TTB Headers
#include "ttb_typedefs.h"
#include "signal.h"
#include "error.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ---------------------- Data Validation ---------------------
// ------------------------------------------------------------
void Error::check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes) {
    ivl_scope_type_t scope_type           = IVL_SCT_MODULE;
    string           scope_type_name      = "UNKNOWN";
    bool             scope_type_supported = false;

    for (unsigned int i = 0; i < num_scopes; i++){
        // Get scope type
        scope_type = ivl_scope_type(scopes[i]);

        // Check scope is of type "module"
        assert(scope_type == IVL_SCT_MODULE && "UNSUPPORTED: Verilog scope type.\n");

        // Get scope types
        switch (scope_type) {
            case IVL_SCT_MODULE:
                scope_type_name      = "IVL_SCT_MODULE";
                scope_type_supported = true;
                break;
            case IVL_SCT_FUNCTION:
                scope_type_name = "IVL_SCT_FUNCTION";
                break;
            case IVL_SCT_TASK:
                scope_type_name      = "IVL_SCT_TASK";
                // scope_type_supported = true;
                break;
            case IVL_SCT_BEGIN:
                scope_type_name      = "IVL_SCT_BEGIN";
                // scope_type_supported = true;
                break;
            case IVL_SCT_FORK:
                scope_type_name = "IVL_SCT_FORK";
                break;
            case IVL_SCT_GENERATE:
                scope_type_name = "IVL_SCT_GENERATE";
                break;
            case IVL_SCT_PACKAGE:
                scope_type_name = "IVL_SCT_PACKAGE";
                break;
            case IVL_SCT_CLASS:
                scope_type_name = "IVL_SCT_CLASS";
                break;
            default:
                scope_type_name = "UNKNOWN";
                break;
        }   

        // Check if scope type supported 
        if (!scope_type_supported) {
            fprintf(stderr, "NOT-SUPPORTED: verilog scope type: %s", 
                scope_type_name.c_str());

            fprintf(stderr, "File: %s Line: %d\n", 
                ivl_scope_file(scopes[i]), 
                ivl_scope_lineno(scopes[i]));

            exit(NOT_SUPPORTED_ERROR);
        }
    }
}

void Error::check_signal_exists_in_map(sig_map_t signals, ivl_signal_t signal) {
    if (signals.count(signal) > 0) {
        fprintf(stderr, "ERROR: signal (%s) already exists in hashmap.\n", 
            signals[signal]->get_fullname().c_str());

        exit(DUPLICATE_SIGNALS_FOUND_ERROR);
    }
} 

void Error::check_signal_not_arrayed(sig_map_t signals, ivl_signal_t signal) {
    // Check if signal is arrayed
    if (ivl_signal_dimensions(signal) > 0) {
        fprintf(stderr, "NOT-SUPPORTED: arrayed signal (%s -- %d) encountered.\n", 
            signals[signal]->get_fullname().c_str(), ivl_signal_dimensions(signal));
        
        exit(NOT_SUPPORTED_ERROR);
    } else {
        // Confirm that ARRAY_BASE is 0 (should be for non-arrayed signals)
        assert(ivl_signal_array_base(signal) == 0 && 
            "NOT-SUPPORTED: non-arrayed signal with non-zero ARRAY_BASE.");

        // Confirm that ARRAY_COUNT is a (should be for non-arrayed signals)
        assert(ivl_signal_array_count(signal) == 1 && 
            "NOT-SUPPORTED: non-arrayed signal with ARRAY_COUNT != 1.");
    }
}

void Error::check_signal_not_multidimensional(sig_map_t signals, ivl_signal_t signal) {
    // Check if signal is multidimensional
    if (ivl_signal_packed_dimensions(signal) > 1) {
        fprintf(stderr, "NOT-SUPPORTED: multidimensional signal (%s -- %d) encountered.\n", 
            signals[signal]->get_fullname().c_str(), ivl_signal_packed_dimensions(signal));
        exit(NOT_SUPPORTED_ERROR);
    }
}

void Error::check_event_nexus(ivl_nexus_t nexus, ivl_statement_t statement) {
    // // Check event nexus is only of size 1
    // if (ivl_nexus_ptrs(nexus) != 1) {
    //     fprintf(stderr, "NOT-SUPPORTED: multiple (%d) nexus pointers for event nexus. \
    //         \n(File: %s -- Line: %d).\n", 
    //         ivl_nexus_ptrs(nexus), ivl_stmt_file(statement), ivl_stmt_lineno(statement));

    //     exit(PROCEDURAL_CONNECTIONS_ERROR);
    // }

    // Check event nexus points only to signal object(s)
    for (unsigned int i = 0; i < ivl_nexus_ptrs(nexus); i++) {
        if (!ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, i))) {
            fprintf(stderr, "NOT-SUPPORTED: non-signal event nexus pointer. \
                \n(File: %s -- Line: %d).\n", 
                ivl_stmt_file(statement), ivl_stmt_lineno(statement));

            exit(PROCEDURAL_CONNECTIONS_ERROR);
        }
    }   
}

void Error::check_lvals_not_concatenated(unsigned int num_lvals, ivl_statement_t statement) {
    // Check for concatenated lvals
    if (num_lvals > 1) {
        fprintf(stderr, "NOT-SUPPORTED: concatenated lvals (File: %s -- Line: %d).\n", 
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));

        exit(PROCEDURAL_CONNECTIONS_ERROR);
    }
}

void Error::check_lval_not_nested(ivl_lval_t lval, ivl_statement_t statement) {
    // Check if lval is nested
    if (ivl_lval_nest(lval)) {
        fprintf(stderr, "NOT-SUPPORTED: nested lvals (File: %s -- Line: %d).\n", 
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));

        exit(PROCEDURAL_CONNECTIONS_ERROR);
    }
}

void Error::check_lval_not_memory(ivl_lval_t lval, ivl_statement_t statement) {
    // Check if lval is a memory
    if (ivl_lval_idx(lval)) {
        fprintf(stderr, "NOT-SUPPORTED: memory lvals (File: %s -- Line: %d).\n", 
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));

        exit(PROCEDURAL_CONNECTIONS_ERROR);
    }
}

void Error::check_part_select_expr(ivl_obj_type_t obj_type, ivl_statement_t statement) {
    // Check part-select expression is only of type IVL_CONST_EXPR,
    // i.e. it is a constant expression
    if (obj_type != IVL_EXPR) {
        fprintf(stderr, "NOT-SUPPORTED: non-constant expression part-select (File: %s -- Line: %d).\n", 
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));

        exit(PROCEDURAL_CONNECTIONS_ERROR);
    }
}

void Error::check_slice_tracking_stack(unsigned int size, unsigned int size_limit) {
    // Check slice info stack size
    if (size > size_limit) {
        fprintf(stderr, "ERROR: slice info stack size (%u) > size limit (%u).\n", size, size_limit);
        exit(SLICE_TRACKING_ERROR);
    }
}

// ------------------------------------------------------------
// --------------- Error Reporting: Unkown Types --------------
// ------------------------------------------------------------
void Error::unknown_ivl_obj_type(ivl_obj_type_t obj_type) {
    fprintf(stderr, "ERROR: unkown ivl obj type (%d) encountered.\n", obj_type);
    
    exit(NOT_SUPPORTED_ERROR);
}

void Error::unknown_nexus_type() {
    fprintf(stderr, "ERROR: unkown nexus type for nexus.\n");
    
    exit(CONCURRENT_CONNECTIONS_ERROR);
}

void Error::unknown_signal_port_type(ivl_signal_port_t port_type) {
    fprintf(stderr, "ERROR: unkown signal port type (%d).\n", (int) port_type);
    
    exit(CONCURRENT_CONNECTIONS_ERROR); 
}

void Error::unknown_part_select_lpm_type(ivl_lpm_type_t lpm_type) {
    fprintf(stderr, "ERROR: unkown part select LPM type (%d).\n", (int) lpm_type);
    
    exit(CONCURRENT_CONNECTIONS_ERROR);
}

void Error::unknown_statement_type(ivl_statement_type_t statement_type) {
    fprintf(stderr, "ERROR: uknown statement type (%d).\n", (int) statement_type);
    
    exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::unknown_expression_type(ivl_expr_type_t expression_type) {
    fprintf(stderr, "ERROR: uknown expression type (%d).\n", (int) expression_type);

    exit(PROCEDURAL_CONNECTIONS_ERROR);
}

// ------------------------------------------------------------
// ------------------ Error Reporting: Other ------------------
// ------------------------------------------------------------
void Error::not_supported(const char* message) {
    fprintf(stderr, "NOT-SUPPORTED: %s\n", message);
    
    exit(NOT_SUPPORTED_ERROR);
}

void Error::null_ivl_obj_type() {
    fprintf(stderr, "ERROR: node type IVL_NONE encountered.\n");
    
    exit(NOT_SUPPORTED_ERROR);
}

void Error::connecting_signal_not_in_graph(sig_map_t signals, ivl_signal_t source_signal) {
    fprintf(stderr, "ERROR: attempting to connect signal (%s) not in graph.\n", 
        signals[source_signal]->get_fullname().c_str());

    exit(NOT_SUPPORTED_ERROR);
}

void Error::popping_source_signals_queue() {
    fprintf(stderr, "ERROR: attempting to pop more source signals from queue than exist.\n");

    exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::processing_procedural_connections() {
    fprintf(stderr, "ERROR: processing behavioral logic connections.\n");

    exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::non_local_signal_connection() {
    fprintf(stderr, "ERROR: cannot to remove local signal between non-local signals.\n");

    exit(NOT_SUPPORTED_ERROR);
}
