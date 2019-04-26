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
#include <ttb_typedefs.h>
#include <tracker.h>
#include <error.h>

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------

const char* Tracker::get_process_type_as_string(ivl_process_t process) {
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

int Tracker::process_process(ivl_process_t process) {

    fprintf(DEBUG_PRINTS_FILE_PTR, "processing process (%s)\n", 
        get_process_type_as_string(process));

    switch (ivl_process_type(process)) {
        case IVL_PR_ALWAYS:
            // Check if already in process block. 
            // Nested process blocks are not supported.
            if (check_if_inside_ff_block()) {
                Error::not_supported("nested process blocks.");
            }

            // Check that the always block is not an analog process
            if (ivl_process_analog(process)) {
                Error::not_supported("analog IVL_PR_ALWAYS process statement.");
            }

            // Process statment
            process_statement(ivl_process_stmt(process), WS_TAB);

            // Check if inside ff flag is set. 
            // If so, clear it.
            if (check_if_inside_ff_block()) {
                clear_inside_ff_block();
            }

            break;
        case IVL_PR_INITIAL:
            fprintf(DEBUG_PRINTS_FILE_PTR, "WARNING: Ignoring INITIAL block...\n");
            break;
        case IVL_PR_FINAL:
            fprintf(DEBUG_PRINTS_FILE_PTR, "WARNING: Ignoring FINAL block...\n");
            break;
        default:
            Error::not_supported("process statement type (UNKOWN).");
            break;
    }

    return 0;
}
