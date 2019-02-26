/*
File:        process_statement.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signal connections in process blocks.
IVL process blocks include initial, always, and final blocks.
*/

// Standard Headers

// TTB Headers
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
const char* get_statement_type_as_string(ivl_statement_t statement) {
    switch (ivl_statement_type(statement)) {
        case IVL_ST_NONE:
            return "IVL_ST_NONE";
        case IVL_ST_NOOP:
            return "IVL_ST_NOOP";
        case IVL_ST_ALLOC:
            return "IVL_ST_ALLOC";
        case IVL_ST_ASSIGN:
            return "IVL_ST_ASSIGN";
        case IVL_ST_ASSIGN_NB:
            return "IVL_ST_ASSIGN_NB";
        case IVL_ST_BLOCK:
            return "IVL_ST_BLOCK";
        case IVL_ST_CASE:
            return "IVL_ST_CASE";
        case IVL_ST_CASER:
            return "IVL_ST_CASER";
        case IVL_ST_CASEX:
            return "IVL_ST_CASEX";
        case IVL_ST_CASEZ:
            return "IVL_ST_CASEZ";
        case IVL_ST_CASSIGN:
            return "IVL_ST_CASSIGN";
        case IVL_ST_CONDIT:
            return "IVL_ST_CONDIT";
        case IVL_ST_CONTRIB:
            return "IVL_ST_CONTRIB";
        case IVL_ST_DEASSIGN:
            return "IVL_ST_DEASSIGN";
        case IVL_ST_DELAY:
            return "IVL_ST_DELAY";
        case IVL_ST_DELAYX:
            return "IVL_ST_DELAYX";
        case IVL_ST_DISABLE:
            return "IVL_ST_DISABLE";
        case IVL_ST_DO_WHILE:
            return "IVL_ST_DO_WHILE";
        case IVL_ST_FORCE:
            return "IVL_ST_FORCE";
        case IVL_ST_FOREVER:
            return "IVL_ST_FOREVER";
        case IVL_ST_FORK:
            return "IVL_ST_FORK";
        case IVL_ST_FORK_JOIN_ANY:
            return "IVL_ST_FORK_JOIN_ANY";
        case IVL_ST_FORK_JOIN_NONE:
            return "IVL_ST_FORK_JOIN_NONE";
        case IVL_ST_FREE:
            return "IVL_ST_FREE";
        case IVL_ST_RELEASE:
            return "IVL_ST_RELEASE";
        case IVL_ST_REPEAT:
            return "IVL_ST_REPEAT";
        case IVL_ST_STASK:
            return "IVL_ST_STASK";
        case IVL_ST_TRIGGER:
            return "IVL_ST_TRIGGER";
        case IVL_ST_UTASK:
            return "IVL_ST_UTASK";
        case IVL_ST_WAIT:
            return "IVL_ST_WAIT";
        case IVL_ST_WHILE:
            return "IVL_ST_WHILE";
        default:
            return "UNKOWN";
    }
}

// ----------------------------------------------------------------------------------
// --------------------------- SUB-PROCESSING Functions -----------------------------
// ----------------------------------------------------------------------------------
void process_event_nexus(ivl_nexus_t nexus, ivl_statement_t statement, SignalGraph* sg, string ws) {
    // Nexus Pointer
    ivl_nexus_ptr_t nexus_ptr = NULL;

    // Source Signal Object
    ivl_signal_t    source_signal    = NULL;

    // Check no more than one nexus pointer for an event nexus
    // Check nexus pointer type is signal object only
    // @TODO: propgate other nexus types besides signals
    Error::check_event_nexus(nexus, statement);

    // Get event nexus pointer signal object
    source_signal = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, 0));

    // Push signal to source signals queue
    sg->push_to_source_signals_queue(source_signal, ws);
}

void process_statement_wait(ivl_statement_t statement, SignalGraph* sg, string ws) {
    ivl_event_t     event                  = NULL;
    ivl_nexus_t     event_nexus            = NULL;
    ivl_statement_t sub_statement          = NULL;
    unsigned int    num_posedge_nexus_ptrs = 0;
    unsigned int    num_negedge_nexus_ptrs = 0;
    unsigned int    num_anyedge_nexus_ptrs = 0;

    // Get number of WAIT statement events
    unsigned int num_events = ivl_stmt_nevent(statement);

    // Process Event(s)
    // Iterate over statement events
    for (unsigned int i = 0; i < num_events; i++) {

        // Get event
        event = ivl_stmt_events(statement, i);

        // Iterate through nexi associated with an POS-EDGE event
        if ((num_posedge_nexus_ptrs = ivl_event_npos(event))) {
            fprintf(stdout, "%sprocessing event %d @posedge\n", ws.c_str(), i);
            for (unsigned int j = 0; j < num_posedge_nexus_ptrs; j++) {
                event_nexus = ivl_event_pos(event, j);      
                process_event_nexus(event_nexus, statement, sg, ws + "  ");
            }
        }
    
        // Iterate through nexi associated with an NEG-EDGE event
        if ((num_negedge_nexus_ptrs = ivl_event_nneg(event))) {
            fprintf(stdout, "%sprocessing event %d @negedge\n", ws.c_str(), i);
            for (unsigned int j = 0; j < num_negedge_nexus_ptrs; j++) {
                event_nexus = ivl_event_neg(event, j);      
                process_event_nexus(event_nexus, statement, sg, ws + "  ");
            }
        }

        // Iterate through nexi associated with an ANY-EDGE event
         if ((num_anyedge_nexus_ptrs = ivl_event_nany(event))) {
            fprintf(stdout, "%sprocessing event %d @anyedge\n", ws.c_str(), i);
            for (unsigned int j = 0; j < num_anyedge_nexus_ptrs; j++) {
                event_nexus = ivl_event_any(event, j);      
                process_event_nexus(event_nexus, statement, sg, ws + "  ");
            }
        }
    }

    // Get/process sub-statement
    if ((sub_statement = ivl_stmt_sub_stmt(statement))) {
        process_statement(sub_statement, sg, ws);
    }
}

void process_statement_condit(ivl_statement_t statement, SignalGraph* sg, string ws) { 
    // Get conditional expression object
    ivl_expr_t condit_expr = ivl_stmt_cond_expr(statement);

    // Get true and false sub-statements
    ivl_statement_t true_statement  = ivl_stmt_cond_true(statement);
    ivl_statement_t false_statement = ivl_stmt_cond_false(statement);
    
    // Process conditional expression to get source signals
    process_expression(condit_expr, sg, ws);

    // Process true/false sub-statements to propagate 
    // source signals to sink signals
    if (true_statement) {
        process_statement(true_statement, sg, ws);
    }
    if (false_statement) {
        process_statement(false_statement, sg, ws);
    }
}

void process_statement_assign(ivl_statement_t statement, SignalGraph* sg, string ws) {
    ivl_signal_t sink_signal = NULL;
    ivl_signal_t source_signal = NULL;
    ivl_lval_t   lval      = NULL;
    ivl_expr_t   part_select_offset = NULL;
    unsigned int num_lvals = 0;

    // Get number of lvals
    num_lvals = ivl_stmt_lvals(statement);

    // Check for concatendated lvals
    Error::check_lvals_not_concatenated(num_lvals, statement);

    // Process lval
    fprintf(stdout, "%sprocessing (%u) lval(s) ...\n", 
        ws.c_str(), num_lvals);
    // @TODO: support concatenated lvals
    for (unsigned int i = 0; i < num_lvals; i++) {
        // Get lval object
        lval = ivl_stmt_lval(statement, i);

        // Check that lval is NOT nested
        Error::check_lval_not_nested(lval, statement);

        // Get sink signal
        sink_signal = ivl_lval_sig(lval);

        // Process lval expression (if necessary)
        if ((part_select_offset = ivl_lval_part_off(lval))) {
            process_expression(part_select_offset, sg, ws + "  ");
            process_expression(part_select_offset, sg, ws + "  ");
        }
    }

    // Process rval
    fprintf(stdout, "%sprocessing rval ...\n", ws.c_str());
    process_expression(ivl_stmt_rval(statement), sg, ws + "  ");

    // Add connection(s)
    while ((source_signal = sg->pop_from_source_signals_queue())) {
        sg->add_connection(sink_signal, source_signal, ws + "  ");
    }
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------
void process_statement(ivl_statement_t statement, SignalGraph* sg, string ws) {

    fprintf(stdout, "%sprocessing statement (%s)\n", 
        ws.c_str(), get_statement_type_as_string(statement));

    switch (ivl_statement_type(statement)) {
        case IVL_ST_NONE:
            break;

        case IVL_ST_NOOP:
            // DO NOTHING
            break;
        
        case IVL_ST_ASSIGN:
        case IVL_ST_ASSIGN_NB:
            process_statement_assign(statement, sg, ws + "  ");
            break;

        case IVL_ST_BLOCK:
            break;

        case IVL_ST_CASE:
        case IVL_ST_CASER:
        case IVL_ST_CASEX:
        case IVL_ST_CASEZ:
            break;

        case IVL_ST_CASSIGN:
            break;

        case IVL_ST_CONDIT:
            process_statement_condit(statement, sg, ws + "  ");
            break;
        
        case IVL_ST_DELAY:
        case IVL_ST_DELAYX:
            break;
        
        case IVL_ST_WAIT:
            process_statement_wait(statement, sg, ws + "  ");
            break;

        case IVL_ST_ALLOC:
        case IVL_ST_CONTRIB:
        case IVL_ST_DEASSIGN:
        case IVL_ST_DISABLE:
        case IVL_ST_DO_WHILE:
        case IVL_ST_FORCE:
        case IVL_ST_FOREVER:
        case IVL_ST_FORK:
        case IVL_ST_FORK_JOIN_ANY:
        case IVL_ST_FORK_JOIN_NONE:
        case IVL_ST_FREE:
        case IVL_ST_RELEASE:
        case IVL_ST_REPEAT:
        case IVL_ST_STASK:
        case IVL_ST_TRIGGER:
        case IVL_ST_UTASK:
        case IVL_ST_WHILE:
        default:
            Error::unknown_statement_type((unsigned int) ivl_statement_type(statement));
            break;
    }
} 
