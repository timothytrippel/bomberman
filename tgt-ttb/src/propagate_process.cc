/*
File:        propagate_process.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes signal connections in process blocks.
IVL process blocks include initial, always, and final blocks.
*/

// Standard Headers

// TTB Headers
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

void find_behavioral_connections(ivl_design_t design, SignalGraph* sg) {
    int result = 0;

    // Processes are initial, always, or final blocks
    // Goes through all assignments in process blocks.
    result = ivl_design_process(design, propagate_process, sg);
    if (result != 0) {
        Error::processing_behavioral_connections();
    }
}

int process_always_statement(ivl_statement_t statement, SignalGraph* sg, string ws) {
    SignalGraph* signal_graph = (SignalGraph*) sg;

    fprintf(stdout, "%sprocessing ALWAYS block...\n", ws.c_str());

    switch (ivl_statement_type(statement)) {
        case IVL_ST_NONE:
            break;
        case IVL_ST_NOOP:
            // DO NOTHING
            break;
        case IVL_ST_ALLOC:
        case IVL_ST_ASSIGN:
        case IVL_ST_ASSIGN_NB:
        case IVL_ST_BLOCK:
        case IVL_ST_CASE:
        case IVL_ST_CASER:
        case IVL_ST_CASEX:
        case IVL_ST_CASEZ:
        case IVL_ST_CASSIGN:
        case IVL_ST_CONDIT:
        case IVL_ST_CONTRIB:
        case IVL_ST_DEASSIGN:
        case IVL_ST_DELAY:
        case IVL_ST_DELAYX:
        case IVL_ST_DISABLE:
        case IVL_ST_DO_WHILE:
        case IVL_ST_FORCE:
        case IVL_ST_FOREVER:
        case IVL_ST_FORK:
        case IVL_ST_FORK_JOIN_ANY:
        case IVL_ST_FORK_JOIN_NONE:
        case IVL_ST_FREE:
        case IVL_ST_RELEASE:
        case IVL_ST_REPEAT:
        case IVL_ST_STASK:
        case IVL_ST_TRIGGER:
        case IVL_ST_UTASK:
        case IVL_ST_WAIT:
        case IVL_ST_WHILE:
        default:
            Error::unknown_statement_type((unsigned int) ivl_statement_type(statement));
            break;
    }

    return 0;
}   

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESS Progation -------------------------------
// ----------------------------------------------------------------------------------
int propagate_process(ivl_process_t process, void* sg) {

    switch (ivl_process_type(process)) {
        case IVL_PR_ALWAYS:
            
            // Check that the always block is not an analog process
            if (ivl_process_analog(process)) {
                Error::not_supported_error("analog IVL_PR_ALWAYS process statement.");
            }

            process_always_statement(ivl_process_stmt(process), (SignalGraph*) sg, "  ");

            break;
        case IVL_PR_INITIAL:
            Error::not_supported_error("process statement type (IVL_PR_INITIAL).");
            break;
        case IVL_PR_FINAL:
            Error::not_supported_error("process statement type (IVL_PR_FINAL).");
            break;
        default:
            Error::not_supported_error("process statement type (UNKOWN).");
            break;
    }

    return 0;
}
