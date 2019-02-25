/*
File:        propagate_constant.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signals connected to a CONSTANT.
*/

// Standard Headers
// #include <cassert>
#include <cstdio>
#include <string>

// TTB Headers
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
const char* SignalGraph::get_const_type_as_string(ivl_net_const_t constant) {
    switch (ivl_const_type(constant)) {
        case IVL_VT_BOOL:
            return "IVL_VT_BOOL";
        case IVL_VT_LOGIC:
            return "IVL_VT_LOGIC";
        default:
            return "UNKOWN";
    }
}

// ----------------------------------------------------------------------------------
// --------------------------- Main CONSTANT Progation ------------------------------
// ----------------------------------------------------------------------------------
void SignalGraph::propagate_constant(ivl_net_const_t constant,
                                     ivl_signal_t sink_signal, 
                                     string       ws) {

    switch (ivl_const_type(constant)) {
        case IVL_VT_BOOL:
        case IVL_VT_LOGIC:
            // process_constant(constant, sink_nexus, sink_signal, ws);
            add_constant_connection(sink_signal, constant, ws + "   ");
            break;
        default:
            Error::not_supported_error("CONSTANT device type (UNKNOWN)");
            break;
    }
}
