/*
File:        propagate_constant.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signals connected to a CONSTANT.
*/

// Standard Headers

// TTB Headers
#include "ttb_typedefs.h"
#include "tracker.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
const char* Tracker::get_const_type_as_string(ivl_net_const_t constant) {
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
void Tracker::propagate_constant(
    ivl_net_const_t constant,
    Signal*         sink_signal,
    string          ws) {

    // Source signal
    Signal* source_signal = new Signal(constant);

    switch (ivl_const_type(constant)) {
        case IVL_VT_BOOL:
        case IVL_VT_LOGIC:

            // Check that slice-info stacks are correct sizes
            // Source Slices Stack:
            // (Source slice stack should never grow beyond size N, 
            //  where N = number of nodes on source signals queue.)
            Error::check_slice_tracking_stack(source_slices_.get_num_slices(), 1);
            // Sink Slices Stack:
            // (Sink slice stack should never grow beyond size 1.)
            Error::check_slice_tracking_stack(sink_slices_.get_num_slices(), 1);

            // Add Connection
            sg_->add_connection(
                sink_signal, 
                source_signal, 
                get_sink_slice(sink_signal),
                get_source_slice(source_signal),
                ws + WS_TAB);

            break;
        default:
            Error::not_supported("CONSTANT device type (UNKNOWN)");
            break;
    }
}
