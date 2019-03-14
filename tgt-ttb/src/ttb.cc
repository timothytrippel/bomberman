/*
File:        ttb.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <cassert>

// TTB Headers
#include "ttb_typedefs.h"
#include "ttb.h"
#include "signal.h"
#include "reporter.h"
#include "error.h"

// ------------------------------------------------------------
// ------------------ Connection Processing -------------------
// ------------------------------------------------------------

void find_structural_connections(SignalGraph* sg) {

    // Get signals adjacency list
    sig_map_t signals_map = sg->get_signals_map();

    // Create a signals map iterator
    sig_map_t::iterator it = signals_map.begin();
 
    // Iterate over all signals in adjacency list
    while (it != signals_map.end()) {  

        // Print signal name -- signal dimensions
        fprintf(stdout, "%s:\n", it->second->get_fullname().c_str());

        // Get signal nexus
        // There is exactly one nexus for each WORD of a signal.
        // Since we only support non-arrayed signals (above), 
        // each signal only has one nexus.
        const ivl_nexus_t sink_nexus = ivl_signal_nex(it->first, 0);

        // Check Nexus IS NOT NULL
        assert(sink_nexus);

        // Propagate the nexus
        propagate_nexus(sink_nexus, it->second, sg, WS_TAB);

        // Increment the iterator
        it++;
    }
}

void find_behavioral_connections(ivl_design_t design, SignalGraph* sg) {
    int result = 0;

    // Processes are initial, always, or final blocks
    // Goes through all assignments in process blocks.
    result = ivl_design_process(design, process_process, sg);
    if (result != 0) {
        Error::processing_behavioral_connections();
    }
}

// ------------------------------------------------------------
// -------------- Initialize Constant ID Counter --------------
// ------------------------------------------------------------

unsigned int Signal::const_id = 0;

// ------------------------------------------------------------
// -------------- IVL Target Entry-point ("main") -------------
// ------------------------------------------------------------
int target_design(ivl_design_t design) {
    ivl_scope_t* roots     = 0;    // root scopes of the design
    unsigned     num_roots = 0;    // number of root scopes of the design
    Reporter*    reporter  = NULL; // reporter object (prints messages)
    SignalGraph* sg        = NULL; // signal graph object

    // Initialize reporter checking objects
    reporter = new Reporter();
    reporter->init(LAUNCH_MESSAGE);

    // Initialize SignalGraph
    sg = new SignalGraph(ivl_design_flag(design, "-o"));

    
    
    // Get root scopes (top level modules) of design
    reporter->print_message(SCOPE_EXPANSION_MESSAGE);
    ivl_design_roots(design, &roots, &num_roots);
    Error::check_scope_types(roots, num_roots);
    reporter->root_scopes(roots, num_roots);

    // Find all critical signals and dependencies in the design
    reporter->print_message(SIGNAL_ENUM_MESSAGE);
    sg->find_all_signals(roots, num_roots);
    reporter->num_signals(sg->get_num_signals());
    reporter->signal_names(sg->get_signals_map());

    // Find signal-to-signal connections
    reporter->print_message(COMB_CONNECTION_ENUM_MESSAGE);
    find_structural_connections(sg);
    reporter->print_message(BEHAVE_CONNECTION_ENUM_MESSAGE);
    find_behavioral_connections(design, sg);

    // Report Graph Stats
    reporter->print_message(STATS_MESSAGE);
    reporter->graph_stats(sg);

    // Save dot graph to file
    sg->save_dot_graph();

    // Report total execution time
    reporter->end();

    // Delete Objects
    delete(reporter);
    delete(sg);

    return 0;
}
