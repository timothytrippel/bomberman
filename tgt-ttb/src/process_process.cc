/*
File:        process_process.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function processes signal connections in process blocks.
IVL process blocks include initial, always, and final blocks,
i.e. behavioral statement blocks.
*/

// Standard Headers

// TTB Headers
#include "ttb_typedefs.h"
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------

const char* get_process_type_as_string(ivl_process_t process) {
    switch (ivl_process_type(process)) {
        case IVL_PR_ALWAYS:
            return "IVL_PR_ALWAYS";
        case IVL_PR_INITIAL:
            return "IVL_PR_INITIAL";
        case IVL_PR_FINAL:
            return "IVL_PR_FINAL";
        default:
            return "UNKOWN";
    }
}  

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

int process_process(ivl_process_t process, void* sg) {

    switch (ivl_process_type(process)) {
        case IVL_PR_ALWAYS:
        case IVL_PR_INITIAL:
        case IVL_PR_FINAL:

            // Check that the always block is not an analog process
            if (ivl_process_analog(process)) {
                Error::not_supported("analog IVL_PR_ALWAYS process statement.");
            }

            process_statement(ivl_process_stmt(process), (SignalGraph*) sg, WS_TAB);
            break;

        default:
            Error::not_supported("process statement type (UNKOWN).");
            break;
    }

    return 0;
}
