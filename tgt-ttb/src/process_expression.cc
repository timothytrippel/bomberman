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
void process_expression_signal(ivl_expr_t expression, SignalGraph* sg, string ws) {
    // Source (IVL signal/const/const_expr) node
    node_object_t node_obj    = {NULL};
    node_t        source_node = {node_obj, IVL_NONE};

    // Check if signal is arrayed
    Error::check_signal_not_arrayed(ivl_expr_signal(expression));

    // Get expression signal
    source_node.object.ivl_signal = ivl_expr_signal(expression);
    source_node.type              = IVL_SIGNAL;

    // Push source node to source nodes queue
    sg->push_to_source_nodes_queue(source_node, ws + WS_TAB);
}

void process_expression_number(ivl_expr_t expression, SignalGraph* sg, string ws) {
    // Source (IVL signal/const/const_expr) node
    node_object_t node_obj    = {NULL};
    node_t        source_node = {node_obj, IVL_NONE};

    // Get expression signal
    source_node.object.ivl_const_expr = expression;
    source_node.type                  = IVL_CONST_EXPR;

    // Push source node to source nodes queue
    sg->push_to_source_nodes_queue(source_node, ws + WS_TAB);
}

void process_expression_binary(ivl_expr_t expression, SignalGraph* sg, string ws) {
    // Process operand expressions
    process_expression(ivl_expr_oper1(expression), sg, ws + WS_TAB);
    process_expression(ivl_expr_oper2(expression), sg, ws + WS_TAB);
}

void process_expression_ternary(ivl_expr_t expression, SignalGraph* sg, string ws) {
    // Process operand expressions
    process_expression(ivl_expr_oper1(expression), sg, ws + WS_TAB);
    process_expression(ivl_expr_oper2(expression), sg, ws + WS_TAB);
    process_expression(ivl_expr_oper3(expression), sg, ws + WS_TAB);
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------
void process_expression(ivl_expr_t   expression, 
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
            process_expression_binary(expression, sg, ws);
            break;
        case IVL_EX_CONCAT:
            Error::not_supported("expression type (IVL_EX_CONCAT).");
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
            process_expression_number(expression, sg, ws);
            break;
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
            process_expression_signal(expression, sg, ws);
            break;
        case IVL_EX_STRING:
            Error::not_supported("expression type (IVL_EX_STRING).");
            break;
        case IVL_EX_TERNARY:
            process_expression_ternary(expression, sg, ws);
            break;
        case IVL_EX_UFUNC:
            Error::not_supported("expression type (IVL_EX_UFUNC).");
            break;
        case IVL_EX_ULONG:
            Error::not_supported("expression type (IVL_EX_ULONG).");
            break;
        case IVL_EX_UNARY:
            Error::not_supported("expression type (IVL_EX_UNARY).");
            break;
        default:
            Error::unknown_expression_type(ivl_expr_type(expression));
            break;
    }
}
