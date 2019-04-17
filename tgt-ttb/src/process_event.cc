/*
File:        process_event.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

These functions propagte signal connections in event statements,
i.e., @(posedge), @(negedge), or @(anyedge).
*/

// Standard Headers

// TTB Headers
#include "ttb_typedefs.h"
#include "tracker.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// --------------------------- SUB-PROCESSING Functions -----------------------------
// ----------------------------------------------------------------------------------

unsigned int Tracker::process_event_nexus(
    ivl_event_t     event,
    ivl_nexus_t     nexus, 
    ivl_statement_t statement, 
    string          ws) {

    // Source IVL signal
    ivl_signal_t temp_sig          = NULL;
    ivl_signal_t source_ivl_signal = NULL;

    // Get event nexus pointer IVL signal (source signal)
    if (ivl_nexus_ptrs(nexus) == 1) {

        // IVL source signal is at index 0
        source_ivl_signal = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, 0));

    } else if (ivl_nexus_ptrs(nexus) > 1) {

        // Find IVL source signal whose scope matches that of the event itself
        for (unsigned int i = 0; i < ivl_nexus_ptrs(nexus); i++) {

            // Check nexus pointer type(s) are signal object(s) only
            // @TODO: propgate other nexus types besides signals 
            if ((temp_sig = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, i)))) {
                
                // Check if scope of source signal matches scope of event
                if (ivl_signal_scope(temp_sig) == ivl_event_scope(event)) {

                    // Check if source signal already found
                    if (!source_ivl_signal) {
                        source_ivl_signal = temp_sig;
                    } else {
                        Error::multiple_valid_event_nexus_ptrs(statement);
                    }
                }
            } else {
                Error::non_signal_event_nexus_ptr(statement);
            }
        } 
    } else {
        Error::zero_event_nexus_ptrs(statement);
    }

    // Check if CLK signal is one of source signals. If so, 
    // it means we have entered an always block, and subsequent
    // sink signals should be marked as FFs.
    if (check_if_clk_signal(source_ivl_signal)) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocess is clocked\n", ws.c_str());
        set_inside_ff_block();
    } 
    
    // Check if signal is to be ignored
    if (!sg_->check_if_ignore_signal(source_ivl_signal)) {

        // Get signal object from IVL source signal
        Signal* source_signal = sg_->get_signal_from_ivl_signal(source_ivl_signal);

        // Push signal to source signals queue
        push_source_signal(source_signal, 0, ws + WS_TAB);
    }

    return 1;
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------

unsigned int Tracker::process_event(
    ivl_event_t     event, 
    ivl_statement_t statement, 
    string          ws) {

    ivl_nexus_t  event_nexus            = NULL;
    unsigned int num_posedge_nexus_ptrs = 0;
    unsigned int num_negedge_nexus_ptrs = 0;
    unsigned int num_anyedge_nexus_ptrs = 0;
    unsigned int num_nodes_processed    = 0;

    // Iterate through nexi associated with an POS-EDGE event
    if ((num_posedge_nexus_ptrs = ivl_event_npos(event))) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing event @posedge\n", ws.c_str());
        for (unsigned int j = 0; j < num_posedge_nexus_ptrs; j++) {
            event_nexus = ivl_event_pos(event, j);      
            num_nodes_processed += process_event_nexus(
                event, event_nexus, statement, ws);
        }
    }

    // Iterate through nexi associated with an NEG-EDGE event
    if ((num_negedge_nexus_ptrs = ivl_event_nneg(event))) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing event @negedge\n", ws.c_str());
        for (unsigned int j = 0; j < num_negedge_nexus_ptrs; j++) {
            event_nexus = ivl_event_neg(event, j);      
            num_nodes_processed += process_event_nexus(
                event, event_nexus, statement, ws);
        }
    }

    // Iterate through nexi associated with an ANY-EDGE event
     if ((num_anyedge_nexus_ptrs = ivl_event_nany(event))) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocessing event @anyedge\n", ws.c_str());
        for (unsigned int j = 0; j < num_anyedge_nexus_ptrs; j++) {
            event_nexus = ivl_event_any(event, j);      
            num_nodes_processed += process_event_nexus(
                event, event_nexus, statement, ws);
        }
    }

    return num_nodes_processed;
}
