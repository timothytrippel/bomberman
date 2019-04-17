/*
File:        process_expression.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function processes signal connections in expressions.
IVL expressions are usually encountered in behavioral
logic blocks (i.e. initial, always, final blocks), otherwise
expressions are processed as combinational logic.
*/

// Standard Headers

// TTB Headers
#include "ttb_typedefs.h"
#include "tracker.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------

const char* Tracker::get_expr_type_as_string(ivl_expr_t expression) {
    switch (ivl_expr_type(expression)) {
        case IVL_EX_NONE:
            return "IVL_EX_NONE";
        case IVL_EX_ARRAY:
            return "IVL_EX_ARRAY";
        case IVL_EX_BACCESS:
            return "IVL_EX_BACCESS";
        case IVL_EX_BINARY:
            return "IVL_EX_BINARY";
        case IVL_EX_CONCAT:
            return "IVL_EX_CONCAT";
        case IVL_EX_DELAY:
            return "IVL_EX_DELAY";
        case IVL_EX_ENUMTYPE:
            return "IVL_EX_ENUMTYPE";
        case IVL_EX_EVENT:
            return "IVL_EX_EVENT";
        case IVL_EX_MEMORY:
            return "IVL_EX_MEMORY";
        case IVL_EX_NEW:
            return "IVL_EX_NEW";
        case IVL_EX_NULL:
            return "IVL_EX_NULL";
        case IVL_EX_NUMBER:
            return "IVL_EX_NUMBER";
        case IVL_EX_ARRAY_PATTERN:
            return "IVL_EX_ARRAY_PATTERN";
        case IVL_EX_PROPERTY:
            return "IVL_EX_PROPERTY";
        case IVL_EX_REALNUM:
            return "IVL_EX_REALNUM";
        case IVL_EX_SCOPE:
            return "IVL_EX_SCOPE";
        case IVL_EX_SELECT:
            return "IVL_EX_SELECT";
        case IVL_EX_SFUNC:
            return "IVL_EX_SFUNC";
        case IVL_EX_SHALLOWCOPY:
            return "IVL_EX_SHALLOWCOPY";
        case IVL_EX_SIGNAL:
            return "IVL_EX_SIGNAL";
        case IVL_EX_STRING:
            return "IVL_EX_STRING";
        case IVL_EX_TERNARY:
            return "IVL_EX_TERNARY";
        case IVL_EX_UFUNC:
            return "IVL_EX_UFUNC";
        case IVL_EX_ULONG:
            return "IVL_EX_ULONG";
        case IVL_EX_UNARY:
            return "IVL_EX_UNARY";
        default:
            return "UNKOWN";
    }
}

// ----------------------------------------------------------------------------------
// --------------------------- SUB-PROCESSING Functions -----------------------------
// ----------------------------------------------------------------------------------

// ------------------------------ SIGNAL Expression ---------------------------------

unsigned int Tracker::process_expression_signal(
    ivl_expr_t      expression,
    ivl_statement_t statement,
    string          ws) {

    // Source signal object
    Signal* source_signal = NULL;

    // Signal array index expression (arrayed signals)
    unsigned int num_index_exprs = 0;
    unsigned int array_index     = 0;
    Signal*      index_expr      = NULL;

    // Get expression IVL signal (source signal)
    ivl_signal_t source_ivl_signal = ivl_expr_signal(expression);

    // Get arrayed index expression
    num_index_exprs = process_expression(ivl_expr_oper1(expression), statement, ws + WS_TAB);
    
    // Check that array index consists of (only) one expression
    if (num_index_exprs == 1) {

        // Get array index (i.e. source signal ID)
        index_expr  = pop_source_signal();
        array_index = index_expr->process_as_partselect_expr(statement);

    } else {

        // If there is not EXACTLY 1 base expression, there must be none
        assert(num_index_exprs == 0 && 
            "ERROR: non-zero number of signal word index expressions encountered.\n");
    }

    // Check if signal is to be ignored
    if (!sg_->check_if_ignore_signal(source_ivl_signal)) {

        // Get signal object
        source_signal = sg_->get_signal_from_ivl_signal(source_ivl_signal);

        // Push source node to source nodes queue
        push_source_signal(source_signal, array_index, ws + WS_TAB);
    }

    return 1;
}

// ------------------------------ NUMBER Expression ---------------------------------

unsigned int Tracker::process_expression_number(
    ivl_expr_t expression,
    string     ws) {

    // Get expression signal
    Signal* source_signal = new Signal(expression);

    // Push source node to source nodes queue
    push_source_signal(source_signal, 0, ws + WS_TAB);

    return 1;
}

// ------------------------------ SELECT Expression ---------------------------------

unsigned int Tracker::process_expression_select(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    string          ws) {

    // Index of select expression
    Signal* index = NULL;

    // MSB/LSB slice of base computed from index expression
    unsigned int msb = 0;
    unsigned int lsb = 0;

    // Number of signals added to source signals queue
    unsigned int num_base_exprs  = 0;
    unsigned int num_index_exprs = 0;

    // Get select base
    num_base_exprs = process_expression(ivl_expr_oper1(expression), statement, ws + WS_TAB);
    
    // Check that base consists of (only) one IVL expression
    assert(num_base_exprs == 1 && "ERROR: more than one base expr. processed.\n");
    assert(source_signals_.get_back_signal()->is_signal() &&
        "ERROR: expression select base is NOT a signal.\n");

    // Get select index
    num_index_exprs = process_expression(ivl_expr_oper2(expression), statement, ws + WS_TAB);

    // Check that index consists of (only) one IVL expression
    assert((num_index_exprs == 0 || num_index_exprs == 1) && "ERROR: more than one index expr. processed.\n");
    if (num_index_exprs) {
        
        // Get index
        index = pop_source_signal();

        // Get LSB of select
        lsb = index->process_as_partselect_expr(statement);
    }

    // Get MSB of select
    msb = lsb + ivl_expr_width(expression) - 1;

    // Track source slice
    track_source_slice(msb, lsb, ws + WS_TAB);

    return num_base_exprs;
}

// ------------------------------ CONCAT Expression ---------------------------------

unsigned int Tracker::process_expression_concat(
    ivl_expr_t      expression,
    ivl_statement_t statement, 
    string          ws) {

    // Source signals processed here
    unsigned int num_source_signals_processed = 0;
    unsigned int num_total_source_signals     = 0;

    // Bit Slices
    unsigned int current_msb = 0;
    unsigned int current_lsb = 0;

    // Process concatenated expressions
    for (unsigned int i = 0; i < ivl_expr_repeat(expression); i++) {

        // Track bit slices
        // While it is undocumented in ivl_target.h, empiracally it
        // seems the concat inputs are always tracked from MSB->LSB.
        // Hence, processing in reverse to go from LSB->MSB.
        for (int j = (ivl_expr_parms(expression) - 1); j >= 0; j--) {

            // Update MSB
            current_msb = current_lsb + ivl_expr_width(ivl_expr_parm(expression, j)) - 1;

            // Process expression
            num_source_signals_processed = process_expression(
                ivl_expr_parm(expression, j), statement, ws + WS_TAB);

            // Add source slice to queue, but only if we just
            // processed base source signals.
            if (num_source_signals_processed == 1) {
                track_sink_slice(current_msb, current_lsb, ws + WS_TAB);
            }

            // Update total source signals processed
            num_total_source_signals += num_source_signals_processed;

            // Update LSB
            current_lsb = current_msb + 1;
        }      
    }

    return num_total_source_signals;
}

// ------------------------------ UNARY Expression ----------------------------------

unsigned int Tracker::process_expression_unary(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    string          ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expression
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), statement, ws + WS_TAB);

    return num_nodes_processed;
}

// ------------------------------ BINARY Expression ---------------------------------

unsigned int Tracker::process_expression_binary(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    string          ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expressions
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), statement, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper2(expression), statement, ws + WS_TAB);

    return num_nodes_processed;
}

// ----------------------------- TERNARY Expression ---------------------------------

unsigned int Tracker::process_expression_ternary(
    ivl_expr_t      expression,
    ivl_statement_t statement,
    string          ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expressions
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), statement, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper2(expression), statement, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper3(expression), statement, ws + WS_TAB);

    return num_nodes_processed;
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

unsigned int Tracker::process_expression(
    ivl_expr_t      expression,
    ivl_statement_t statement,  
    string          ws) {

    fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing expression (%s)\n", 
        ws.c_str(), get_expr_type_as_string(expression));

    switch (ivl_expr_type(expression)) {
        case IVL_EX_NONE:
            // do nothing
            break;
        case IVL_EX_ARRAY:
            Error::not_supported("expression type (IVL_EX_ARRAY).");
            break;
        case IVL_EX_BACCESS:
            Error::not_supported("expression type (IVL_EX_BACCESS).");
            break;
        case IVL_EX_BINARY:
            return process_expression_binary(expression, statement, ws);
        case IVL_EX_CONCAT:
            return process_expression_concat(expression, statement, ws);
            break;
        case IVL_EX_DELAY:
            Error::not_supported("expression type (IVL_EX_DELAY).");
            break;
        case IVL_EX_ENUMTYPE:
            Error::not_supported("expression type (IVL_EX_ENUMTYPE).");
            break;
        case IVL_EX_EVENT:
            Error::not_supported("expression type (IVL_EX_EVENT).");
            break;
        case IVL_EX_MEMORY:
            Error::not_supported("expression type (IVL_EX_MEMORY).");
            break;
        case IVL_EX_NEW:
            Error::not_supported("expression type (IVL_EX_NEW).");
            break;
        case IVL_EX_NULL:
            Error::not_supported("expression type (IVL_EX_NULL).");
            break;
        case IVL_EX_NUMBER:
            return process_expression_number(expression, ws);
        case IVL_EX_ARRAY_PATTERN:
            Error::not_supported("expression type (IVL_EX_ARRAY_PATTERN).");
            break;
        case IVL_EX_PROPERTY:
            Error::not_supported("expression type (IVL_EX_PROPERTY).");
            break;
        case IVL_EX_REALNUM:
            Error::not_supported("expression type (IVL_EX_REALNUM).");
            break;
        case IVL_EX_SCOPE:
            Error::not_supported("expression type (IVL_EX_SCOPE).");
            break;
        case IVL_EX_SELECT:
            return process_expression_select(expression, statement, ws);
        case IVL_EX_SFUNC:
            Error::not_supported("expression type (IVL_EX_SFUNC).");
            break;
        case IVL_EX_SHALLOWCOPY:
            Error::not_supported("expression type (IVL_EX_SHALLOWCOPY).");
            break;
        case IVL_EX_SIGNAL:
            return process_expression_signal(expression, statement, ws);
        case IVL_EX_STRING:
            Error::not_supported("expression type (IVL_EX_STRING).");
            break;
        case IVL_EX_TERNARY:
            return process_expression_ternary(expression, statement, ws);
        case IVL_EX_UFUNC:
            Error::not_supported("expression type (IVL_EX_UFUNC).");
            break;
        case IVL_EX_ULONG:
            Error::not_supported("expression type (IVL_EX_ULONG).");
            break;
        case IVL_EX_UNARY:
            return process_expression_unary(expression, statement, ws);
        default:
            Error::unknown_expression_type(ivl_expr_type(expression));
            break;
    }

    return 0;
}
