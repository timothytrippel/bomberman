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

// ----------------------------------------------------------------------------------
// --------------------------- SUB-PROCESSING Functions -----------------------------
// ----------------------------------------------------------------------------------

// ------------------------------ SIGNAL Expression ---------------------------------

// ivl_signal_t process_expression_local_signal(ivl_signal_t local_signal, 
//                                              SignalGraph* sg, 
//                                              string       ws) {

//     // Source signal object
//     ivl_signal_t source_signal = NULL;
//     ivl_nexus_t  nexus         = NULL; // nexus connected to local signal

//     fprintf(stdout, "%sprocessing local signal (%s)\n", 
//         ws.c_str(), ivl_signal_basename(local_signal));
    
//     // Get signal nexus
//     // There is exactly one nexus for each WORD of a signal.
//     // Since we only support non-arrayed signals (above), 
//     // each signal only has one nexus.
//     nexus = ivl_signal_nex(local_signal, 0);

//     // Check nexus is not NULL
//     assert(nexus && "ERROR: source nexus is not valid");

//     unsigned int num_nexus_ptrs = ivl_nexus_ptrs(nexus);
//     fprintf(stdout, "%sNum. source nexus ptrs: %u\n", ws.c_str(), num_nexus_ptrs);

    




//     // Nexus Pointer
//     ivl_nexus_ptr_t nexus_ptr = NULL;

//     // Iterate over Nexus pointers in Nexus
//     for (unsigned int nexus_ind = 0; nexus_ind < ivl_nexus_ptrs(nexus); nexus_ind++) {
        
//         nexus_ptr = ivl_nexus_ptr(nexus, nexus_ind);
//         fprintf(stdout, "%sNexus %d", ws.c_str(), nexus_ind);

//         // Determine type of Nexus
//         if ((source_signal = ivl_nexus_ptr_sig(nexus_ptr))){
            
//             // Nexus target object is a SIGNAL
//             fprintf(stdout, " -- SIGNAL -- %s\n", 
//                 ivl_signal_basename(source_signal));   
            
//             // // propagate_signal(source_signal, sink_signal);

//             // // BASE-CASE:
//             // // If connected signal and signal the same, 
//             // // IGNORE, probably a module hookup
//             // // @TODO: investigate this
//             // // Ignore connections to local (IVL generated) signals.
//             // if (source_signal != sink_signal->get_ivl_signal()) {
//             //     sg->add_connection(
//             //         sink_signal, 
//             //         sg->get_signal_from_ivl_signal(source_signal), 
//             //         ws + WS_TAB);
//             // }

//         } 
//         // else if ((source_logic = ivl_nexus_ptr_log(nexus_ptr))) {
            
//         //     // Nexus target object is a LOGIC
//         //     fprintf(stdout, " -- LOGIC -- %s\n", get_logic_type_as_string(source_logic));
//         //     propagate_logic(source_logic, nexus, sink_signal, sg, ws);

//         // } else if ((source_lpm = ivl_nexus_ptr_lpm(nexus_ptr))) {
            
//         //     // Nexus target object is a LPM
//         //     fprintf(stdout, " -- LPM -- %s\n", get_lpm_type_as_string(source_lpm));
//         //     propagate_lpm(source_lpm, nexus, sink_signal, sg, ws);

//         // } else if ((source_constant = ivl_nexus_ptr_con(nexus_ptr))) {
            
//         //     // Nexus target object is a CONSTANT
//         //     fprintf(stdout, " -- CONSTANT -- %s\n", get_const_type_as_string(source_constant));
//         //     propagate_constant(source_constant, sink_signal, sg, ws);

//         // } 

//         else {
            
//             // Nexus target object is UNKNOWN
//             Error::unknown_nexus_type();

//         }
//     }

//     return source_signal;
// }

unsigned int process_expression_signal(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws) {
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

unsigned int process_expression_number(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws) {

    // Get expression signal
    Signal* source_signal = new Signal(expression);

    // Push source node to source nodes queue
    sg->push_to_source_signals_queue(source_signal, ws + WS_TAB);

    return 1;
}

// ------------------------------ CONCAT Expression ---------------------------------

unsigned int process_expression_concat(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process concatenated expressions
    for (unsigned int i = 0; i < ivl_expr_repeat(expression); i++) {
        for (unsigned int j = 0; j < ivl_expr_parms(expression); j++) {
            num_nodes_processed += process_expression(
                ivl_expr_parm(expression, j), sg, ws + WS_TAB);
        }      
    }

    return num_nodes_processed;
}

// ------------------------------ UNARY Expression ----------------------------------

unsigned int process_expression_unary(ivl_expr_t   expression, 
                                      SignalGraph* sg, 
                                      string       ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expression
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), sg, ws + WS_TAB);

    return num_nodes_processed;
}

// ------------------------------ BINARY Expression ---------------------------------

unsigned int process_expression_binary(ivl_expr_t   expression, 
                                       SignalGraph* sg, 
                                       string       ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expressions
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), sg, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper2(expression), sg, ws + WS_TAB);

    return num_nodes_processed;
}

// ----------------------------- TERNARY Expression ---------------------------------

unsigned int process_expression_ternary(ivl_expr_t   expression, 
                                        SignalGraph* sg, 
                                        string       ws) {

    // Source nodes processed here
    unsigned int num_nodes_processed = 0;

    // Process operand expressions
    num_nodes_processed += process_expression(
        ivl_expr_oper1(expression), sg, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper2(expression), sg, ws + WS_TAB);
    num_nodes_processed += process_expression(
        ivl_expr_oper3(expression), sg, ws + WS_TAB);

    return num_nodes_processed;
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

unsigned int process_expression(ivl_expr_t   expression, 
                                SignalGraph* sg,
                                string       ws) {

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
            return process_expression_binary(expression, sg, ws);
        case IVL_EX_CONCAT:
            return process_expression_concat(expression, sg, ws);
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
            return process_expression_number(expression, sg, ws);
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
            Error::not_supported("expression type (IVL_EX_SELECT).");
            break;
        case IVL_EX_SFUNC:
            Error::not_supported("expression type (IVL_EX_SFUNC).");
            break;
        case IVL_EX_SHALLOWCOPY:
            Error::not_supported("expression type (IVL_EX_SHALLOWCOPY).");
            break;
        case IVL_EX_SIGNAL:
            return process_expression_signal(expression, sg, ws);
        case IVL_EX_STRING:
            Error::not_supported("expression type (IVL_EX_STRING).");
            break;
        case IVL_EX_TERNARY:
            return process_expression_ternary(expression, sg, ws);
        case IVL_EX_UFUNC:
            Error::not_supported("expression type (IVL_EX_UFUNC).");
            break;
        case IVL_EX_ULONG:
            Error::not_supported("expression type (IVL_EX_ULONG).");
            break;
        case IVL_EX_UNARY:
            return process_expression_unary(expression, sg, ws);
        default:
            Error::unknown_expression_type(ivl_expr_type(expression));
            break;
    }

    return 0;
}
