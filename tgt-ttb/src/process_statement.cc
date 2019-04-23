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
#include "tracker.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
const char* Tracker::get_statement_type_as_string(ivl_statement_t statement) {
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

void Tracker::process_statement_wait(
    ivl_statement_t statement, 
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
        num_nodes_processed += process_event(event, statement, ws + WS_TAB);
    }

    // Push number of source signals processed at this depth
    push_scope_depth(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source signal(s) to stack\n", 
        ws.c_str(), num_nodes_processed);

    // Get/process sub-statement
    if ((sub_statement = ivl_stmt_sub_stmt(statement))) {
        process_statement(sub_statement, ws + WS_TAB);
    }

    // Pop processed source signals from stack
    num_nodes_processed = pop_scope_depth();
    pop_source_signals(num_nodes_processed, ws);
}

// ------------------------------ CONDIT Statement ----------------------------------

void Tracker::process_statement_condit(
    ivl_statement_t statement,
    string          ws) {

    // Source nodes processed at this here
    unsigned int num_nodes_processed = 0;

    // Get conditional expression object
    ivl_expr_t condit_expr = ivl_stmt_cond_expr(statement);

    // Get true and false sub-statements
    ivl_statement_t true_statement  = ivl_stmt_cond_true(statement);
    ivl_statement_t false_statement = ivl_stmt_cond_false(statement);
    
    // Process conditional expression to get source signals
    num_nodes_processed += process_expression(condit_expr, statement, ws + WS_TAB);
    
    // Push number of source signals processed at this depth
    push_scope_depth(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source signaks(s) to stack\n", 
        ws.c_str(), num_nodes_processed);

    // Process true/false sub-statements to propagate 
    // source signals to sink signals
    if (true_statement) {
        process_statement(true_statement, ws + WS_TAB);
    }
    if (false_statement) {
        process_statement(false_statement, ws + WS_TAB);
    }

    // Pop processed source signals from stack
    num_nodes_processed = pop_scope_depth();
    pop_source_signals(num_nodes_processed, ws);
}

// ------------------------------ ASSIGN Statement ----------------------------------

void Tracker::process_statement_assign_rval(
    Signal*         sink_signal,
    ivl_statement_t statement,
    string          ws) {

    unsigned int num_nodes_processed = 0;    // source nodes processed here
    Signal*      source_signal       = NULL; // source node to connect to

    // Process rval expression
    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing rval(s) ...\n", ws.c_str());
    num_nodes_processed = process_expression(ivl_stmt_rval(statement), statement, ws + WS_TAB);

    // Push number of source nodes processed at this depth
    push_scope_depth(num_nodes_processed);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source signal(s) to queue\n", 
        ws.c_str(), num_nodes_processed);

    // Add connection(s)
    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing connections ...\n", ws.c_str());
    for (int i = (source_signals_.get_num_signals() - 1); i >= 0; i--) {

        // Get source signal
        source_signal = get_source_signal(i);
        // printf("Source Signal Ptr: %d\n", source_signal);

        // signal_slice_t rval_sink_slice = source_signal->get_sink_slice(sink_signal);

        // Only update slices of source signals in current scope depth
        if (i >= (source_signals_.get_num_signals() - num_nodes_processed)) {
            
            // LVal (sink signal) slice is held in the sink signal object.
            // need to move this information to the source signal, since
            // that's where its extracted from when add_connection is called.
            if (sink_signal->is_sink_slice_modified()) { 

                // Get the LVal (sink signal) sink slice
                signal_slice_t lval_sink_slice = sink_signal->get_sink_slice(sink_signal);
                unsigned int lval_lsb = lval_sink_slice.lsb;
                // printf("LVal LSB: %u\n", lval_lsb);

                // Check if the source signal sink slice was modified,
                // i.e., if a concat was processed in the RVal.
                if (source_signal->is_sink_slice_modified()) {

                    // Shift the source signal sink slice to account for LVal offset
                    shift_sink_slice(source_signal, lval_lsb, ws);

                } else {

                    // Otherwise, just move LVal sink slice info to source signal
                    set_sink_slice(source_signal, lval_sink_slice, ws);
                }
            }
        }

        // Check if connection contains IVL generated signals.
        // If so, temporarily store the connections and process
        // them later. Otherwise, process normally.
        if (sink_signal->is_ivl_generated()) {

            fprintf(DEBUG_PRINTS_FILE_PTR, 
                "%slval is IVL generated, storing connection...\n", ws.c_str());

            sg_->track_local_signal_connection(
                sink_signal, 
                source_signal,
                source_signal->get_sink_slice(sink_signal),
                source_signal->get_source_slice(source_signal),
                ws + WS_TAB);

        } else if (source_signal->is_ivl_generated()) {

            fprintf(DEBUG_PRINTS_FILE_PTR, 
                "%srval is IVL generated, storing connection...\n", ws.c_str());

            sg_->track_local_signal_connection(
                sink_signal, 
                source_signal,
                source_signal->get_sink_slice(sink_signal),
                source_signal->get_source_slice(source_signal),
                ws + WS_TAB);

        } else {

            sg_->add_connection(
                sink_signal, 
                source_signal, 
                source_signal->get_sink_slice(sink_signal),
                source_signal->get_source_slice(source_signal),
                ws + WS_TAB);
        }
    }

    // Pop processed source signals from queue
    num_nodes_processed = pop_scope_depth();
    pop_source_signals(num_nodes_processed, ws);
}

void Tracker::process_statement_assign_lval(
    ivl_statement_t statement,
    string          ws) {

    ivl_lval_t   lval              = NULL; // LVal that contains sink signal
    Signal*      sink_signal       = NULL; // sink signal
    ivl_expr_t   array_index_expr  = NULL; // LVal array index expression
    ivl_expr_t   part_select_expr  = NULL; // LVal part-select offset expressions
    unsigned int num_array_signals = 0;    // number of LVal array index signals
    unsigned int array_index       = 0;    // LVal array index
    unsigned int part_select_msb   = 0;    // LVal (sink signal) MSB
    unsigned int part_select_lsb   = 0;    // LVal (sink signal) LSB
    unsigned int num_lvals         = 0;    // number of lvals to process
    vector<unsigned int> array_indexi;     // LVal array indexe

    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing lval(s) ...\n", ws.c_str());

    // Get number of lvals
    num_lvals = ivl_stmt_lvals(statement);

    // Check for (NON-SUPPORTED) concatenated lvals
    // @TODO: support concatenated lvals
    Error::check_lvals_not_concatenated(num_lvals, statement);

    // Get lval object
    lval = ivl_stmt_lval(statement, STMT_ASSIGN_LVAL_INDEX);

    // Get MSB of lval
    part_select_msb = part_select_lsb + ivl_lval_width(lval) - 1;

    // Check for a (NON-SUPPORTED) nested lval
    // @TODO: support nested lvals
    Error::check_lval_not_nested(lval, statement);

    // Get sink signal
    sink_signal = sg_->get_signal_from_ivl_signal(ivl_lval_sig(lval));

    // Reset sink signal slices
    sink_signal->reset_slices();

    // Set sink signal as FF if inside an FF block
    if (check_if_inside_ff_block()) {
        sink_signal->set_is_ff();
    }

    // Check if memory lval (i.e. lval is an arrayed signal)
    if ((array_index_expr = ivl_lval_idx(lval))) {
        
        // Get LVAL signal array indexi
        array_indexi = process_array_index_expression(ivl_lval_sig(lval), array_index_expr, statement, ws + WS_TAB);

        // Check if array index was a signal or a constant
        if (array_index_is_signal_) {

            // number of array index signals processed (max of 1)
            num_array_signals = 1;
            
            // Reset array index is signal flag
            array_index_is_signal_ = false;

        } else {

            // number of array index signals processed (0 for constants)
            num_array_signals = 0;
        }

    } else {

        // number of array index signals processed (max of 1)
        num_array_signals = 0;

        // index is 0 if LVal (sink) signal is not arrayed
        array_indexi.push_back(0);    
    }

    // Push number of array source signals processed at this depth
    // (NOTE: max of 1 currently is supported.)
    push_scope_depth(num_array_signals);
    fprintf(DEBUG_PRINTS_FILE_PTR, "%spushed %d source signal(s) to queue\n", 
                ws.c_str(), num_array_signals);

    // Process lval part select expression (if necessary)
    if ((part_select_expr = ivl_lval_part_off(lval))) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing lval part select ...\n", ws.c_str());

        // Get part-select MSB/LSB
        part_select_lsb = process_index_expression(part_select_expr, statement, ws + WS_TAB);
        part_select_msb = part_select_lsb + ivl_lval_width(lval) - 1;

        // Track sink slice
        set_sink_slice(sink_signal, part_select_msb, part_select_lsb, ws);
    }

    // Iterate over array indexi
    while (array_indexi.size()) {

        // Get (single) LVAL signal array index
        array_index = array_indexi.back();
        array_indexi.pop_back();

        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing lval array index (%d) ...\n", ws.c_str(), array_index);

        // Set sink signal ID
        sink_signal->set_id(array_index);

        // Print LVal sink signal selects
        fprintf(DEBUG_PRINTS_FILE_PTR, "%ssink signal: %s[%d:%d]\n", 
            ws.c_str(),
            sink_signal->get_fullname().c_str(),
            part_select_msb,
            part_select_lsb);

        // Process RVals associated with current LVal
        process_statement_assign_rval(sink_signal, statement, ws);
    }

    // Pop processed (array index) source signals from queue
    num_array_signals = pop_scope_depth();
    pop_source_signals(num_array_signals, ws);
}

void Tracker::process_statement_assign(
    ivl_statement_t statement, 
    string          ws) {

    // Set slice tracking flags
    enable_slicing();

    // Process lval expressions
    process_statement_assign_lval(statement, ws + WS_TAB);

    // Clear slice tracking flags
    disable_slicing();
}

// ------------------------------- BLOCK Statement ----------------------------------

void Tracker::process_statement_block(
    ivl_statement_t statement, 
    string          ws) {

    // Iterate over sub-statements in block
    for (unsigned int i = 0; i < ivl_stmt_block_count(statement); i++) {
        process_statement(ivl_stmt_block_stmt(statement, i), ws + WS_TAB);
    }
}

// ------------------------------- CASE Statement -----------------------------------

void Tracker::process_statement_case(
    ivl_statement_t statement, 
    string          ws) {

    // Iterate over sub-statements in block
    for (unsigned int i = 0; i < ivl_stmt_case_count(statement); i++) {
        process_statement(ivl_stmt_case_stmt(statement, i), ws + WS_TAB);
    }
}

// ------------------------------- DELAY Statement ----------------------------------

void Tracker::process_statement_delay(
    ivl_statement_t statement, 
    string          ws) {

    // Sub-statement
    ivl_statement_t sub_statement = NULL;

    // Check for a sub-statement
    if ((sub_statement = ivl_stmt_sub_stmt(statement))) {
        process_statement(sub_statement, ws + WS_TAB);
    }
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

void Tracker::process_statement(
    ivl_statement_t statement, 
    string          ws) {

    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing statement (%s)\n", 
        ws.c_str(), get_statement_type_as_string(statement));

    switch (ivl_statement_type(statement)) {

        case IVL_ST_NOOP:
            // DO NOTHING
            break;
        
        case IVL_ST_ASSIGN:
        case IVL_ST_ASSIGN_NB:
            process_statement_assign(statement, ws + WS_TAB);
            break;

        case IVL_ST_BLOCK:
            process_statement_block(statement, ws);
            break;

        case IVL_ST_CASE:
        case IVL_ST_CASER:
        case IVL_ST_CASEX:
        case IVL_ST_CASEZ:
            process_statement_case(statement, ws);
            break;

        case IVL_ST_CASSIGN:
            Error::unknown_statement_type(ivl_statement_type(statement));
            break;

        case IVL_ST_CONDIT:
            process_statement_condit(statement, ws);
            break;
        
        case IVL_ST_DELAY:
        case IVL_ST_DELAYX:
            process_statement_delay(statement, ws);
            break;
        
        case IVL_ST_WAIT:
            process_statement_wait(statement, ws);
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
        case IVL_ST_TRIGGER:
        case IVL_ST_STASK:
        case IVL_ST_UTASK:
        case IVL_ST_WHILE:
        default:
            Error::unknown_statement_type(ivl_statement_type(statement));
            break;
    }
} 
