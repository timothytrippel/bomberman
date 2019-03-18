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
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------

const char* get_expr_type_as_string(ivl_expr_t expression) {
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

unsigned int process_expression_partselect(
    Signal*         part_select,                                        
    ivl_statement_t statement) {

    // Check part_select is only of type IVL_CONST_EXPR
    // @TODO: support non-constant part selects,
    // e.g. signals: signal_a[signal_b] <= signal_c;
    Error::check_part_select_expr(part_select->get_ivl_type(), statement);

    // Get part-select constant expression
    ivl_expr_t expr = part_select->get_ivl_obj().ivl_expr;

    // Get LSB offset index
    string bit_string = string(ivl_expr_bits(expr));
    reverse(bit_string.begin(), bit_string.end());

    // Convert bitstring to unsigned long
    return stoul(bit_string, NULL, BITSTRING_BASE);
}

// ----------------------------------------------------------------------------------
// --------------------------- SUB-PROCESSING Functions -----------------------------
// ----------------------------------------------------------------------------------

// ------------------------------ SIGNAL Expression ---------------------------------

unsigned int process_expression_signal(
    ivl_expr_t      expression,
    ivl_statement_t statement,
    SignalGraph*    sg, 
    string          ws) {

    // Source signal object
    Signal* source_signal = NULL;

    // Get expression IVL signal (source signal)
    ivl_signal_t source_ivl_signal = ivl_expr_signal(expression);

    // Check if signal is to be ignored
    if (!sg->check_if_ignore_signal(source_ivl_signal)) {
        
        // Check if signal is arrayed
        Error::check_signal_not_arrayed(sg->get_signals_map(), source_ivl_signal);

        // Get signal object
        source_signal = sg->get_signal_from_ivl_signal(source_ivl_signal);

        // Push source node to source nodes queue
        sg->push_to_source_signals_queue(source_signal, ws + WS_TAB);
    }

    return 1;
}

// ------------------------------ NUMBER Expression ---------------------------------

unsigned int process_expression_number(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    SignalGraph*    sg, 
    string          ws) {

    // Get expression signal
    Signal* source_signal = new Signal(expression);

    // Push source node to source nodes queue
    sg->push_to_source_signals_queue(source_signal, ws + WS_TAB);

    return 1;
}

// ------------------------------ SELECT Expression ---------------------------------

unsigned int process_expression_select(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    SignalGraph*    sg, 
    string          ws) {

    // Base and Index of select expression
    Signal* base  = NULL;
    Signal* index = NULL;

    // MSB/LSB slice of base computed from index expression
    unsigned int msb = 0;
    unsigned int lsb = 0;

    // Number of signals added to source signals queue
    unsigned int num_base_exprs  = 0;
    unsigned int num_index_exprs = 0;

    // Get select base
    num_base_exprs = process_expression(ivl_expr_oper1(expression), statement, sg, ws + WS_TAB);
    
    // Check that base consists of (only) one IVL expression
    assert(num_base_exprs == 1 && "ERROR: more than one base expr. processed.\n");
    assert(sg->get_source_signals_queue().back()->is_signal() &&
        "ERROR: expression select base is NOT a signal.\n");

    // Get select index
    num_index_exprs = process_expression(ivl_expr_oper2(expression), statement, sg, ws + WS_TAB);

    // Check that index consists of (only) one IVL expression
    assert(num_index_exprs == 1 && "ERROR: more than one index expr. processed.\n");
    index = sg->pop_from_source_signals_queue();

    // Get LSB and MSB of select
    lsb = process_expression_partselect(index, statement);
    msb = lsb + ivl_expr_width(expression) - 1;

    // Add source slice (of base) to queue
    sg->track_source_slice(msb, lsb, ws + WS_TAB);

    return num_base_exprs;
}

// ------------------------------ CONCAT Expression ---------------------------------

unsigned int process_expression_concat(
    ivl_expr_t      expression,
    ivl_statement_t statement, 
    SignalGraph*    sg, 
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
                ivl_expr_parm(expression, j), statement, sg, ws + WS_TAB);

            // Add source slice to queue, but only if we just
            // processed base source signals.
            if (num_source_signals_processed == 1) {
                sg->track_sink_slice(current_msb, current_lsb, ws + WS_TAB);
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

unsigned int process_expression_unary(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    SignalGraph*    sg, 
    string          ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expression
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), statement, sg, ws + WS_TAB);

    return num_nodes_processed;
}

// ------------------------------ BINARY Expression ---------------------------------

unsigned int process_expression_binary(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    SignalGraph*    sg, 
    string          ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expressions
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), statement, sg, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper2(expression), statement, sg, ws + WS_TAB);

    return num_nodes_processed;
}

// ----------------------------- TERNARY Expression ---------------------------------

unsigned int process_expression_ternary(
    ivl_expr_t      expression,
    ivl_statement_t statement,
    SignalGraph*    sg, 
    string          ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expressions
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), statement, sg, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper2(expression), statement, sg, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper3(expression), statement, sg, ws + WS_TAB);

    return num_nodes_processed;
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

unsigned int process_expression(
    ivl_expr_t      expression,
    ivl_statement_t statement,  
    SignalGraph*    sg,
    string          ws) {

    fprintf(stdout, "%sprocessing expression (%s)\n", 
        ws.c_str(), get_expr_type_as_string(expression));

    switch (ivl_expr_type(expression)) {
        case IVL_EX_NONE:
            Error::not_supported("expression type (IVL_EX_NONE).");
            break;
        case IVL_EX_ARRAY:
            Error::not_supported("expression type (IVL_EX_ARRAY).");
            break;
        case IVL_EX_BACCESS:
            Error::not_supported("expression type (IVL_EX_BACCESS).");
            break;
        case IVL_EX_BINARY:
            return process_expression_binary(expression, statement, sg, ws);
        case IVL_EX_CONCAT:
            return process_expression_concat(expression, statement, sg, ws);
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
            return process_expression_number(expression, statement, sg, ws);
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
            return process_expression_select(expression, statement, sg, ws);
        case IVL_EX_SFUNC:
            Error::not_supported("expression type (IVL_EX_SFUNC).");
            break;
        case IVL_EX_SHALLOWCOPY:
            Error::not_supported("expression type (IVL_EX_SHALLOWCOPY).");
            break;
        case IVL_EX_SIGNAL:
            return process_expression_signal(expression, statement, sg, ws);
        case IVL_EX_STRING:
            Error::not_supported("expression type (IVL_EX_STRING).");
            break;
        case IVL_EX_TERNARY:
            return process_expression_ternary(expression, statement, sg, ws);
        case IVL_EX_UFUNC:
            Error::not_supported("expression type (IVL_EX_UFUNC).");
            break;
        case IVL_EX_ULONG:
            Error::not_supported("expression type (IVL_EX_ULONG).");
            break;
        case IVL_EX_UNARY:
            return process_expression_unary(expression, statement, sg, ws);
        default:
            Error::unknown_expression_type(ivl_expr_type(expression));
            break;
    }

    return 0;
}
