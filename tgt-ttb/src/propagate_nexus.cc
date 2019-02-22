/*
File:        propagate_nexus.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes a root signal through a nexus. A nexus
is how IVL connects all data-structures (signals, logic-devices,
LPMs, and constant ). Think of a nexus like a pin, connecting
nets and devices together. Each nexus is associated with a 
single IVL data-structure, where data-structures can have
multiple nexis associated with them. Nexi contain muliplte 
nexus pointers that point to other nexus. Every nexus contains 
a nexus pointer to itself, and the rest are pointers to all the 
IVL data-structures it connects. 
*/

// Standard Headers

// TTB Headers
#include "ttb.h"
#include "error.h"

void SignalGraph::propagate_nexus(ivl_nexus_t nexus, ivl_signal_t sink_signal, string ws) {
    // Nexus Pointer
    ivl_nexus_ptr_t nexus_ptr = NULL;

    // Connected Objects
    ivl_signal_t    source_signal    = NULL;
    ivl_net_logic_t source_logic     = NULL;
    ivl_lpm_t       source_lpm       = NULL;
    ivl_net_const_t source_constant  = NULL;

    // Iterate over Nexus pointers in Nexus
    for (unsigned int nexus_ind = 0; nexus_ind < ivl_nexus_ptrs(nexus); nexus_ind++) {
        nexus_ptr = ivl_nexus_ptr(nexus, nexus_ind);
        fprintf(stdout, "%sNexus %d", ws.c_str(), nexus_ind);

        // Determine type of Nexus
        if ((source_signal = ivl_nexus_ptr_sig(nexus_ptr))){
            // Nexus target object is a SIGNAL
            fprintf(stdout, " -- SIGNAL -- %s\n", get_signal_fullname(source_signal).c_str());   
            // propagate_signal(source_signal, sink_signal);

            // BASE-CASE:
            // If connected signal and signal the same, 
            // IGNORE, probably a module hookup
            // @TODO: investigate this
            // Ignore connections to local (IVL generated) signals.
            if (source_signal != sink_signal) {
                add_connection(sink_signal, source_signal, ws + "  ");
                num_connections_++;
            }
        } else if ((source_logic = ivl_nexus_ptr_log(nexus_ptr))) {
            // Nexus target object is a LOGIC
            fprintf(stdout, " -- LOGIC -- %s\n", get_logic_type_as_string(source_logic));
            propagate_logic(source_logic, nexus, sink_signal, ws);
        } else if ((source_lpm = ivl_nexus_ptr_lpm(nexus_ptr))) {
            // Nexus target object is a LPM
            fprintf(stdout, " -- LPM -- %s\n", get_lpm_type_as_string(source_lpm));
            propagate_lpm(source_lpm, nexus, sink_signal, ws);
        } else if ((source_constant = ivl_nexus_ptr_con(nexus_ptr))) {
            // Nexus target object is a CONSTANT
            fprintf(stdout, " -- CONSTANT -- %s\n", get_const_type_as_string(source_constant));
            propagate_constant(source_constant, nexus, sink_signal, ws);
        } else {
            // Nexus target object is UNKNOWN
            Error::unknown_nexus_type_error();
        }
    }
}
