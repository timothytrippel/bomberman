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
// -------------- CMD Line Arguments Processing ---------------
// ------------------------------------------------------------

cmd_args_map_t* process_cmd_line_args(ivl_design_t des) {
    // Create map to hold cmd line args
    cmd_args_map_t* cmd_args = new cmd_args_map_t();

    // Process output filename
    string output_filename = string(ivl_design_flag(des, OUTPUT_FILENAME_FLAG));
    if (!output_filename.empty()) {
        (*cmd_args)[OUTPUT_FILENAME_FLAG] = output_filename; 
    } else {
        Error::not_supported("output filename is required input.\n");
    } 

    // Process CLK signal basename
    string clk_basename = string(ivl_design_flag(des, CLK_BASENAME_FLAG));
    if (!clk_basename.empty()) {
        (*cmd_args)[CLK_BASENAME_FLAG] = clk_basename; 
    } else {
        Error::not_supported("CLK signal name is required input.\n");
    } 

    // Process signals to ignore filepath
    string ignore_filepath = string(ivl_design_flag(des, IGNORE_FILEPATH_FLAG));
    if (!ignore_filepath.empty()) {
        (*cmd_args)[IGNORE_FILEPATH_FLAG] = ignore_filepath; 
    }
    
    // Process ignore constants flag
    string ignore_consts = string(ivl_design_flag(des, IGNORE_CONSTANTS_FLAG));
    if (!ignore_consts.empty()) {
        (*cmd_args)[IGNORE_CONSTANTS_FLAG] = ignore_consts;
    }

    return cmd_args;
}

// ------------------------------------------------------------
// ----------------- Continuous HDL Processing ----------------
// ------------------------------------------------------------

void find_continuous_connections(SignalGraph* sg) {

    // Signal object pointer
    Signal* current_signal = NULL;

    // Get signals adjacency list
    sig_map_t signals_map = sg->get_signals_map();

    // Create a signals map iterator
    sig_map_t::iterator it = signals_map.begin();
 
    // Iterate over all signals in adjacency list
    while (it != signals_map.end()) {  

        // Get Signal object
        current_signal = it->second;

        // Only find connections to non-ivl-generated signals
        if (!current_signal->is_ivl_generated()) {

            // Print signal name and port type
            fprintf(DEBUG_PRINTS_FILE_PTR, "%s (%s):\n", 
                current_signal->get_fullname().c_str(),
                get_signal_port_type_as_string(current_signal->get_ivl_signal()));

            // Get signal nexus
            // There is exactly one nexus for each WORD of a signal.
            // Since we only support non-arrayed signals (above), 
            // each signal only has one nexus.
            const ivl_nexus_t sink_nexus = ivl_signal_nex(it->first, 0);

            // Check nexus is not NULL
            assert(sink_nexus && "ERROR: sink nexus is not valid.\n");

            // Propagate the nexus
            propagate_nexus(sink_nexus, it->second, sg, WS_TAB);
        }

        // Increment the iterator
        it++;
    }
}

// ------------------------------------------------------------
// ----------------- Procedural HDL Processing ----------------
// ------------------------------------------------------------

void find_procedural_connections(ivl_design_t design, SignalGraph* sg) {
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
    ivl_scope_t*    roots     = 0;    // root scopes of the design
    unsigned        num_roots = 0;    // number of root scopes of the design
    cmd_args_map_t* cmd_args  = NULL; // command line args
    Reporter*       reporter  = NULL; // reporter object (prints messages)
    SignalGraph*    sg        = NULL; // signal graph object

    // Create reporter processing object
    reporter = new Reporter(REPORTER_PRINTS_FILE_PTR);
    
    // Start timer
    reporter->start_task(LAUNCH_MESSAGE);

    // Get IVL design flags (CMD-line args)
    reporter->start_task(CONFIGS_MESSAGE);
    cmd_args = process_cmd_line_args(design);
    reporter->configurations(cmd_args);
    reporter->end_task();

    // Initialize SignalGraph
    reporter->start_task(INITIALIZE_SIG_GRAPH_MESSAGE);
    sg = new SignalGraph(cmd_args);
    reporter->end_task();
    
    // Get root scopes (top level modules) of design
    reporter->start_task(SCOPE_EXPANSION_MESSAGE);
    ivl_design_roots(design, &roots, &num_roots);
    Error::check_scope_types(roots, num_roots);
    reporter->root_scopes(roots, num_roots);
    reporter->end_task();

    // Find all SIGNALS in the design
    reporter->start_task(SIGNAL_ENUM_MESSAGE);
    sg->find_all_signals(roots, num_roots);
    reporter->num_signals(sg->get_num_signals());
    reporter->signal_names(sg->get_signals_map());
    reporter->end_task();

    // Find CONTINUOUS signal-to-signal CONNECTIONS
    reporter->start_task(COMB_CONNECTION_ENUM_MESSAGE);
    find_continuous_connections(sg);
    reporter->end_task();

    // Find PROCEDURAL signal-to-signal CONNECTIONS
    reporter->start_task(BEHAVE_CONNECTION_ENUM_MESSAGE);
    find_procedural_connections(design, sg);
    reporter->end_task();

    // Process connections through local (IVL-generated) signals
    reporter->start_task(LOCAL_CONNECTION_OPT_MESSAGE);
    sg->process_local_connections(WS_TAB);
    reporter->end_task();

    // Write signal nodes to dot graph and save
    reporter->start_task(SIGNAL_SAVING_MESSAGE);
    sg->write_signals_to_dot_graph();
    sg->save_dot_graph();
    reporter->end_task();

    // Report stats/total execution time
    reporter->print_message(FINAL_STATS_MESSAGE);
    reporter->graph_stats(sg);
    reporter->end_task();
    
    // Delete Objects
    reporter->start_task(DESTROY_MESSAGE);
    delete(cmd_args);
    delete(sg);
    reporter->end_task();
    reporter->line_separator();
    delete(reporter);

    return 0;
}
