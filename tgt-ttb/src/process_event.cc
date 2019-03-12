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
#include "ttb.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// --------------------------- SUB-PROCESSING Functions -----------------------------
// ----------------------------------------------------------------------------------
void process_event_nexus(ivl_nexus_t     nexus, 
                         ivl_statement_t statement, 
                         SignalGraph*    sg, 
                         string          ws) {

    // Source Signal Object
    node_object_t node_obj    = {NULL};
    node_t        source_node = {node_obj, IVL_NONE};

    // Check no more than one nexus pointer for an event nexus
    // Check nexus pointer type is signal object only
    // @TODO: propgate other nexus types besides signals
    Error::check_event_nexus(nexus, statement);

    // Get event nexus pointer signal object
    source_node.object.ivl_signal = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, 0));
    source_node.type              = IVL_SIGNAL;

    // Push signal to source signals queue
    sg->push_to_source_nodes_queue(source_node, ws + WS_TAB);
}

// ----------------------------------------------------------------------------------
// --------------------------- Main PROCESSING Function -----------------------------
// ----------------------------------------------------------------------------------
void process_event(ivl_event_t     event, 
                   ivl_statement_t statement, 
                   SignalGraph*    sg, 
                   string          ws) {

    ivl_nexus_t  event_nexus            = NULL;
    unsigned int num_posedge_nexus_ptrs = 0;
    unsigned int num_negedge_nexus_ptrs = 0;
    unsigned int num_anyedge_nexus_ptrs = 0;

    // Iterate through nexi associated with an POS-EDGE event
    if ((num_posedge_nexus_ptrs = ivl_event_npos(event))) {
        fprintf(stdout, "%sprocessing event @posedge\n", ws.c_str());
        for (unsigned int j = 0; j < num_posedge_nexus_ptrs; j++) {
            event_nexus = ivl_event_pos(event, j);      
            process_event_nexus(event_nexus, statement, sg, ws);
        }
    }

    // Iterate through nexi associated with an NEG-EDGE event
    if ((num_negedge_nexus_ptrs = ivl_event_nneg(event))) {
        fprintf(stdout, "%sprocessing event @negedge\n", ws.c_str());
        for (unsigned int j = 0; j < num_negedge_nexus_ptrs; j++) {
            event_nexus = ivl_event_neg(event, j);      
            process_event_nexus(event_nexus, statement, sg, ws);
        }
    }

    // Iterate through nexi associated with an ANY-EDGE event
     if ((num_anyedge_nexus_ptrs = ivl_event_nany(event))) {
        fprintf(stdout, "%sprocessing event @anyedge\n", ws.c_str());
        for (unsigned int j = 0; j < num_anyedge_nexus_ptrs; j++) {
            event_nexus = ivl_event_any(event, j);      
            process_event_nexus(event_nexus, statement, sg, ws);
        }
    }
}
