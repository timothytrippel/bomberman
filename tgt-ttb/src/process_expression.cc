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
#include <cmath>
#include <typeinfo>

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

    // Signal array index
    vector<unsigned int> array_indexi;

    // Get expression IVL signal (source signal)
    ivl_signal_t source_ivl_signal = ivl_expr_signal(expression);

    // Get signal array index
    array_indexi = process_array_index_expression(source_ivl_signal, ivl_expr_oper1(expression), statement, ws + WS_TAB);

    // Check if signal is a mem signal (not in the signals map)
    if (!sg_->check_if_ignore_mem_signal(source_ivl_signal)) {

        // Get signal object
        source_signal = sg_->get_signal_from_ivl_signal(source_ivl_signal);

        // Iterate over array indexi
        for (unsigned int i = 0; i < array_indexi.size(); i++) {
            
            // Push source node to source nodes queue
            push_source_signal(source_signal, array_indexi[i], ws + WS_TAB);
        }

        // Check if array index was a signal or a constant
        if (array_index_is_signal_) {
            array_index_is_signal_ = false;
            return (array_indexi.size() + 1);
        } else {
            return 1;    
        }
    } else {
        return 0;
    }
}

// ------------------------------ NUMBER Expression ---------------------------------

unsigned int Tracker::process_expression_number(
    ivl_expr_t expression,
    string     ws) {

    // Get expression signal
    Signal* source_signal = new Signal(expression);

    // if (source_signal->get_fullname().find(string("const.3886.00010101000000000000000000000011")) != string::npos) {
    //     exit(1);
    // }

    // Push source node to source nodes queue
    push_source_signal(source_signal, 0, ws + WS_TAB);

    return 1;
}

// ------------------------------ SELECT Expression ---------------------------------

unsigned int Tracker::process_expression_select(
    ivl_expr_t      expression, 
    ivl_statement_t statement,
    string          ws) {

    // Base signal of select expression
    Signal* potential_base = NULL;
    Signal* base           = NULL;

    // MSB/LSB slice of base computed from index expression
    int msb = 0;
    int lsb = 0;

    // Number of base signals added to source signals queue
    // Note: should only be one
    unsigned int num_base_exprs    = 0;
    unsigned int num_consts_popped = 0;

    // Process base expression
    num_base_exprs = process_expression(ivl_expr_oper1(expression), statement, ws + WS_TAB);

    // Select base signal
    fprintf(DEBUG_PRINTS_FILE_PTR, "%snum base exprs: %d\n", ws.c_str(), num_base_exprs);
    for (unsigned int i = 0; i < num_base_exprs; i++) {

        // Get potential base signal
        potential_base = source_signals_.get_back_signal(i);

        fprintf(DEBUG_PRINTS_FILE_PTR, "%spotential base signal:    %s\n", 
            ws.c_str(), potential_base->get_fullname().c_str());

        // Check if potential_base is a signal (not a constant)
        if (potential_base->is_signal()) {

            // Check that the potential base has not yet been found
            if (!base) {

                // Assign base signal
                base = source_signals_.get_back_signal(i);
                fprintf(DEBUG_PRINTS_FILE_PTR, "%sbase signal identified:   %s\n", 
                    ws.c_str(), base->get_fullname().c_str());

            } else {

                // NOT SUPPORTED
                assert(false &&
                    "ERROR-Tracker::process_expression_select: more than one possible base expr. processed.\n");
            }
        }
    }

    // Pop all base source signals from source signals queue
    for (unsigned int i = 0; i < num_base_exprs; i++) {
        
        // Pop potential base signal
        potential_base = pop_source_signal(ws);

        // Free the memory if it is a constant
        if (!potential_base->is_signal()) {

            delete(potential_base);
            potential_base = NULL;
            num_consts_popped++;

        } else if (potential_base != base) {

            // ERROR if there is more than one potential base signal
            assert(false && "ERROR-Tracker::process_expression_select: more than one possible base expr. to pop.\n");
        }
    }
    
    // Check that the base signal is NOT arrayed
    if (!base->is_arrayed()) {

        // Re-push base signal to source signals queue
        push_source_signal(base, 0, ws);

    } else {

        // ERROR on nested array selects (i.e. a base signal that is arrayed)
        assert(false && "ERROR-Tracker::process_expression_select: nested array selects not supported.\n");
    }

    // Update number of base expressions (signals) processed
    num_base_exprs -= num_consts_popped;

    // Check that base IVL signal was found
    assert(base && "ERROR-Tracker::process_expression_select: expression select base signal not found.\n");

    // Get select LSB
    lsb = process_index_expression(ivl_expr_oper2(expression), statement, ws + WS_TAB);

    // Get MSB of select
    msb = lsb + ivl_expr_width(expression) - 1;

    fprintf(DEBUG_PRINTS_FILE_PTR, "%sindex select:   [%u:%u]\n", ws.c_str(), msb, lsb);

    // Check if MSB is greater than MSB of base signal and LSB
    // is less than LSB of base signal. If yes, do NOT set source
    // slice as this is a nested select index of an arrayed signal.
    signal_slice_t slice = base->get_source_slice(base);
    if (msb <= (int) slice.msb && lsb >= (int) slice.lsb) {
        
        fprintf(DEBUG_PRINTS_FILE_PTR, "%supdating source slice with index select...\n", ws.c_str());
        
        // Track source slice
        set_source_slice(base, msb, lsb, ws);
    }
    
    return num_base_exprs;
}

// ------------------------------ CONCAT Expression ---------------------------------

unsigned int Tracker::process_expression_concat(
    ivl_expr_t      expression,
    ivl_statement_t statement, 
    string          ws) {

    // Base source signal
    Signal* base = NULL;

    // Source signal pointer
    Signal* source_signal = NULL;

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

            // Track sink slice information, but only if we just
            // processed base source signals.
            if (num_source_signals_processed == 1) {

                // Get base source signal
                base = source_signals_.get_back_signal();

                // Track sink slice
                set_sink_slice(base, current_msb, current_lsb, ws);

            } else {

                // Update nested slices
                for (unsigned int k = 1; k <= num_source_signals_processed; k++) {
                    source_signal = source_signals_.get_signal(source_signals_.get_num_signals() - k);
                    shift_sink_slice(source_signal, current_lsb, ws);
                }
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

// ------------------------------- INDEX Expression ---------------------------------

vector<unsigned int> Tracker::process_array_index_expression(
    ivl_signal_t    base_ivl_signal,
    ivl_expr_t      expression,
    ivl_statement_t statement,  
    string          ws) {

    unsigned int         num_signals_processed   = 0;
    unsigned int         num_array_inds          = 0;
    Signal*              index_signal            = NULL;
    signal_slice_t       source_slice            = {0, 0};
    vector<unsigned int> indexi;

    // Check if expression type is IVL_EX_NONE
    if (ivl_expr_type(expression) == IVL_EX_NONE) {
        indexi.push_back(0);
        return indexi;
    }

    // Process index expression
    num_signals_processed = process_expression(expression, statement, ws);
    
    // Check that only one signal was processed
    assert(num_signals_processed == 1 && 
        "ERROR-Tracker::process_array_index_expression: more than one index expr. processed.\n");

    // Get signal constant that is the result 
    // of processing the index expression
    index_signal = source_signals_.get_back_signal();

    // Check if index is a SIGNAL or a CONSTANT
    if (index_signal->is_const_expr()) {
        
        // Convert the signal constant to a number
        indexi.push_back(index_signal->to_uint());

        // Pop the index signal from the source signals queue
        pop_source_signal(ws);

        // Free signal constant memory
        // (Note: memory only allocated for a new constant signal object)
        delete(index_signal);  

        // Set array index flag
        array_index_is_signal_ = false;

    } else if (index_signal->is_signal()) {

        // Get signal width
        source_slice   = index_signal->get_source_slice(index_signal);
        num_array_inds = pow(2, source_slice.msb - source_slice.lsb + 1);

        // Check that number of array indexi is possible
        fprintf(DEBUG_PRINTS_FILE_PTR, "%snum indicies (%d) / num possible indicies (%d)\n", 
            ws.c_str(), 
            num_array_inds, 
            ivl_signal_array_count(base_ivl_signal));
        assert(num_array_inds <= ivl_signal_array_count(base_ivl_signal) && 
            "ERROR-Tracker::process_array_index_expression: some array indices not possible.\n");

        // Push all possible array indexi to vector
        for (unsigned int i = 0; i < num_array_inds; i++) {
            indexi.push_back(i);
        }

        // Set array index flag
        array_index_is_signal_ = true;

    } else {
        assert(false && "ERROR-Tracker::process_array_index_expression: constant array index not supported\n");
    }

    return indexi;
}

int Tracker::process_index_expression(
    ivl_expr_t      expression,
    ivl_statement_t statement,  
    string          ws) {

    unsigned int num_signals_processed = 0;
    Signal*      index_signal          = NULL;
    unsigned int index                 = 0;

    // Check if expression type is IVL_EX_NONE
    if (ivl_expr_type(expression) == IVL_EX_NONE) {
        return index;
    }

    // Process index expression
    num_signals_processed = process_expression(expression, statement, ws);

    // Check that only one signal was processed
    assert(num_signals_processed == 1 && 
        "ERROR-Tracker::process_index_expression: more than one index expr. processed.\n");

    // Get signal constant that is the result 
    // of processing the index expression
    index_signal = pop_source_signal(ws);

    // Check that index is a constant (not another signal)
    // @TODO: support non-constant indexi,
    // e.g. signals: signal_a[signal_b] <= signal_c;
    // --OR--
    // e.g. signals: signal_a <= signal_c[signal_b];
    // Error::check_part_select_expr(index_signal->get_ivl_type(), statement);

    // Check if the index is a signal or a constant
    if (index_signal->is_const_expr()) { 

        // Convert the signal constant to a number
        index = index_signal->to_uint();

        // Free signal constant memory
        delete(index_signal);   

    } else if (index_signal->is_signal()) {
        // assert(false && "ERROR-Tracker::process_index_expression: signal index not supported\n");
        index = -1;
    } else {
        assert(false && "ERROR-Tracker::process_index_expression: constant index not supported\n");
    }

    return index;
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
