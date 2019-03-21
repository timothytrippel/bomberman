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
#include "ttb_typedefs.h"
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

// ------------------------------- WAIT Statement -----------------------------------

void process_statement_wait(
    ivl_statement_t statement, 
    SignalGraph*    sg, 
    string          ws) {

    ivl_event_t     event               = NULL; // event to be processed
    ivl_statement_t sub_statement       = NULL; // sub-statement to be processed
    unsigned int    num_nodes_processed = 0;    // source nodes processed here

    // Get number of WAIT statement events
    unsigned int num_events = ivl_stmt_nevent(statement);

    // Process Event(s)
    // Iterate over statement events
    for (unsigned int i = 0; i < num_events; i++) {

        // Get event
        event = ivl_stmt_events(statement, i);

        // Process event
        num_nodes_processed += process_event(event, statement, sg, ws + WS_TAB);
    }

    // Push number of source nodes processed at this depth
    sg->push_to_num_signals_at_depth_queue(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source node(s) to queue\n", 
        ws.c_str(), num_nodes_processed);

    // Get/process sub-statement
    if ((sub_statement = ivl_stmt_sub_stmt(statement))) {
        process_statement(sub_statement, sg, ws + WS_TAB);
    }

    // Pop processed source nodes from queue
    num_nodes_processed = sg->pop_from_num_signals_at_depth_queue();
    sg->pop_from_source_signals_queue(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spopped %d source node(s) from queue\n", 
        ws.c_str(), num_nodes_processed);

}

// ------------------------------ CONDIT Statement ----------------------------------

void process_statement_condit(
    ivl_statement_t statement, 
    SignalGraph*    sg, 
    string          ws) {

    // Source nodes processed at this here
    unsigned int num_nodes_processed = 0;

    // Get conditional expression object
    ivl_expr_t condit_expr = ivl_stmt_cond_expr(statement);

    // Get true and false sub-statements
    ivl_statement_t true_statement  = ivl_stmt_cond_true(statement);
    ivl_statement_t false_statement = ivl_stmt_cond_false(statement);
    
    // Process conditional expression to get source signals
    num_nodes_processed += process_expression(condit_expr, statement, sg, ws + WS_TAB);
    
    // Push number of source nodes processed at this depth
    sg->push_to_num_signals_at_depth_queue(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source node(s) to queue\n", 
        ws.c_str(), num_nodes_processed);

    // Process true/false sub-statements to propagate 
    // source signals to sink signals
    if (true_statement) {
        process_statement(true_statement, sg, ws + WS_TAB);
    }
    if (false_statement) {
        process_statement(false_statement, sg, ws + WS_TAB);
    }

    // Pop processed source nodes from queue
    num_nodes_processed = sg->pop_from_num_signals_at_depth_queue();
    sg->pop_from_source_signals_queue(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spopped %d source node(s) from queue\n", 
        ws.c_str(), num_nodes_processed);
}

// ------------------------------ ASSIGN Statement ----------------------------------

Signal* process_statement_assign_lval(
    ivl_statement_t statement,
    SignalGraph*    sg,
    string          ws) {

    ivl_lval_t   lval                = NULL; // lval that contains sink signal
    ivl_expr_t   part_select_expr    = NULL; // lval part-select offset expression
    unsigned int part_select_sources = 0;    // number of part select source exprs processed
    unsigned int part_select_msb     = 0;    // lval (sink signal) MSB
    unsigned int part_select_lsb     = 0;    // lval (sink signal) LSB
    unsigned int num_lvals           = 0;    // number of lvals to process
    Signal*      sink_signal;                // "signal" that contains lval offset expr
    Signal*      part_select;                // "signal" that contains lval offset expr

    // Get number of lvals
    num_lvals = ivl_stmt_lvals(statement);

    // Check for (NON-SUPPORTED) concatenated lvals
    // @TODO: support concatenated lvals
    Error::check_lvals_not_concatenated(num_lvals, statement);

    // Get lval object
    // @TODO: support concatenated lvals
    lval = ivl_stmt_lval(statement, STMT_ASSIGN_LVAL_INDEX);

    // Get MSB of lval
    part_select_msb = part_select_lsb + ivl_lval_width(lval) - 1;

    // Check for a (NON-SUPPORTED) nested lval
    // @TODO: support nested lvals
    Error::check_lval_not_nested(lval, statement);

    // Check for (NON-SUPPORTED) memory lvals
    // @TODO: support memory lvals
    Error::check_lval_not_memory(lval, statement);

    // Get sink signal
    sink_signal = sg->get_signal_from_ivl_signal(ivl_lval_sig(lval));
    
    // Set sink signal as FF if inside an FF block
    if (sg->check_if_inside_ff_block()) {
        sink_signal->set_is_ff();
    }

    // Process lval part select expression (if necessary)
    if ((part_select_expr = ivl_lval_part_off(lval))) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing lval part select ...\n", ws.c_str());

        // Get part-select as constant expression (Signal).
        // Note: Number of source signals added to queue should be 1,
        // because non-constant lval offsets are not supported.
        part_select_sources = process_expression(part_select_expr, statement, sg, ws + WS_TAB);
        assert(part_select_sources == 1 && "ERROR: more than one part select expr. processed.\n");
        part_select = sg->pop_from_source_signals_queue();

        // Update MSB and LSB of slice
        part_select_lsb = part_select->process_as_partselect_expr(statement);
        part_select_msb = part_select_lsb + ivl_lval_width(lval) - 1;

        // Track connection slice information
        sg->track_sink_slice(part_select_msb, part_select_lsb, ws + WS_TAB);

        // Free memory
        delete(part_select);
    }

    // Print LVal sink signal selects
    fprintf(DEBUG_PRINTS_FILE_PTR, "%ssink signal: %s[%d:%d]\n", 
        ws.c_str(),
        sink_signal->get_fullname().c_str(),
        part_select_msb,
        part_select_lsb);

    return sink_signal;
}

void process_statement_assign(
    ivl_statement_t statement, 
    SignalGraph*    sg, 
    string          ws) {

    unsigned int num_nodes_processed = 0;    // source nodes processed here
    Signal*      sink_signal         = NULL; // sink signal to connect to
    Signal*      source_signal       = NULL; // source node to connect to

    // Set slice tracking flags
    sg->set_all_slice_tracking_flags();

    // Process lval expression
    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing lval(s) ...\n", ws.c_str());
    sink_signal = process_statement_assign_lval(statement, sg, ws + WS_TAB);

    // Process rval expression
    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing rval(s) ...\n", ws.c_str());
    num_nodes_processed += process_expression(ivl_stmt_rval(statement), statement, sg, ws + WS_TAB);

    // Push number of source nodes processed at this depth
    sg->push_to_num_signals_at_depth_queue(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source node(s) to queue\n", 
        ws.c_str(), num_nodes_processed);

    // Process Adjustments to LVal slice(s), in the case
    // that the RVal expression contains a concat.
    if (sg->get_num_sink_slices() > num_nodes_processed) {
        sg->erase_index_from_sink_slices(0);
    }

    // Check that slice-info stacks are correct sizes
    // Source Slices Stack:
    // (Source slice stack should never grow beyond size N, 
    //  where N = number of nodes on source signals queue.)
    Error::check_slice_tracking_stack(sg->get_num_source_slices(), num_nodes_processed);
    // Sink Slices Stack:
    // (Sink slice stack should never grow beyond size N, 
    //  where N = number of nodes on source signals queue.)
    Error::check_slice_tracking_stack(sg->get_num_sink_slices(), num_nodes_processed);

    // Add connection(s)
    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing connections ...\n", ws.c_str());
    for (int i = (sg->get_num_source_signals() - 1); i >= 0; i--) {

        source_signal = sg->get_source_signal(i);

        // Check if connection contains IVL generated signals.
        // If so, temporarily store the connections and process
        // them later. Otherwise, process normally.
        if (sink_signal->is_ivl_generated()) {
            fprintf(DEBUG_PRINTS_FILE_PTR, "%slval is IVL generated, storing connection...\n", ws.c_str());
            sg->track_local_signal_connection(sink_signal, source_signal, ws + WS_TAB);
        } else if (source_signal->is_ivl_generated()) {
            fprintf(DEBUG_PRINTS_FILE_PTR, "%srval is IVL generated, storing connection...\n", ws.c_str());
            sg->track_local_signal_connection(sink_signal, source_signal, ws + WS_TAB);
        } else {
            sg->add_connection(sink_signal, source_signal, ws + WS_TAB);
        }

        // Pop slices from stacks
        sg->pop_from_source_slices_queue();
        sg->pop_from_sink_slices_queue();
    }

    // Pop processed source nodes from queue
    num_nodes_processed = sg->pop_from_num_signals_at_depth_queue();
    sg->pop_from_source_signals_queue(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spopped %d source node(s) from queue\n", 
        ws.c_str(), num_nodes_processed);

    // Clear slice tracking flags
    sg->clear_all_slice_tracking_flags();
}

// ------------------------------- BLOCK Statement ----------------------------------

void process_statement_block(
    ivl_statement_t statement, 
    SignalGraph*    sg, 
    string          ws) {

    // Iterate over sub-statements in block
    for (unsigned int i = 0; i < ivl_stmt_block_count(statement); i++) {
        process_statement(ivl_stmt_block_stmt(statement, i), sg, ws + WS_TAB);
    }
}

// ------------------------------- CASE Statement -----------------------------------

void process_statement_case(
    ivl_statement_t statement, 
    SignalGraph*    sg, 
    string          ws) {

    // Iterate over sub-statements in block
    for (unsigned int i = 0; i < ivl_stmt_case_count(statement); i++) {
        process_statement(ivl_stmt_case_stmt(statement, i), sg, ws + WS_TAB);
    }
}

// ------------------------------- DELAY Statement ----------------------------------

void process_statement_delay(
    ivl_statement_t statement, 
    SignalGraph*    sg, 
    string          ws) {

    // Sub-statement
    ivl_statement_t sub_statement = NULL;

    // Check for a sub-statement
    if ((sub_statement = ivl_stmt_sub_stmt(statement))) {
        process_statement(sub_statement, sg, ws + WS_TAB);
    }
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

void process_statement(ivl_statement_t statement, 
                       SignalGraph*    sg, 
                       string          ws) {

    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing statement (%s)\n", 
        ws.c_str(), get_statement_type_as_string(statement));

    switch (ivl_statement_type(statement)) {
        case IVL_ST_NOOP:
            // DO NOTHING
            break;
        
        case IVL_ST_ASSIGN:
        case IVL_ST_ASSIGN_NB:
            process_statement_assign(statement, sg, ws + WS_TAB);
            break;

        case IVL_ST_BLOCK:
            process_statement_block(statement, sg, ws);
            break;

        case IVL_ST_CASE:
        case IVL_ST_CASER:
        case IVL_ST_CASEX:
        case IVL_ST_CASEZ:
            process_statement_case(statement, sg, ws);
            break;

        case IVL_ST_CASSIGN:
            Error::unknown_statement_type(ivl_statement_type(statement));
            break;

        case IVL_ST_CONDIT:
            process_statement_condit(statement, sg, ws);
            break;
        
        case IVL_ST_DELAY:
        case IVL_ST_DELAYX:
            process_statement_delay(statement, sg, ws);
            break;
        
        case IVL_ST_WAIT:
            process_statement_wait(statement, sg, ws);
            break;

        case IVL_ST_NONE:
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
            Error::unknown_statement_type(ivl_statement_type(statement));
            break;
    }
} 
