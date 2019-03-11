/*
File:        propagate_constant.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signals connected to a CONSTANT.
*/

// Standard Headers

// TTB Headers
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
const char* get_const_type_as_string(ivl_net_const_t constant) {
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
void propagate_constant(ivl_net_const_t constant,
                        ivl_signal_t    sink_signal,
                        SignalGraph*    sg,
                        string          ws) {

    // Source node
    node_object_t node_obj    = {.ivl_constant = constant};
    node_t        source_node = {node_obj, IVL_CONST};

    switch (ivl_const_type(constant)) {
        case IVL_VT_BOOL:
        case IVL_VT_LOGIC:
            sg->add_connection(sink_signal, source_node, ws + WS_TAB);
            break;
        default:
            Error::not_supported("CONSTANT device type (UNKNOWN)");
            break;
    }
}
