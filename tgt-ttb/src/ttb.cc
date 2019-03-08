/*
File:        ttb.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

// Standard Headers
#include <cassert>
#include <cstdio>
#include <sstream>

// TTB Headers
#include "ttb.h"
#include "reporter.h"
#include "error.h"

// ----------------------------------------------------------------------------------
// ------------------------------- Helper Functions ---------------------------------
// ----------------------------------------------------------------------------------
const char* get_node_type_as_string(node_type_t node_type) {
    switch(node_type) {
        case IVL_NONE:
            return "IVL_NONE";
        case IVL_SIGNAL:
            return "IVL_SIGNAL";
        case IVL_CONST:
            return "IVL_CONST";
        case IVL_CONST_EXPR:
            return "IVL_CONST_EXPR";
        default:
            return "UNKOWN";
    }
}

string get_signal_fullname(ivl_signal_t signal) {
    string scopename = ivl_scope_name(ivl_signal_scope(signal)); 
    string basename  = ivl_signal_basename(signal);
    string fullname  = scopename + string(".") + basename;

    return fullname;
}

string get_constant_fullname(ivl_net_const_t constant, unsigned long num_constants) {
    // string scopename = ivl_scope_name(ivl_const_scope(constant)); 
    string basename  = string(ivl_const_bits(constant), (size_t)ivl_const_width(constant));
    reverse(basename.begin(), basename.end());
    // string fullname  = scopename + string(".const_") + basename;
    string fullname  = string("const_") + to_string(num_constants) + string(".") + basename;

    return fullname;
}

string get_constant_expr_fullname(ivl_expr_t const_expr, unsigned long num_constants) {
    string basename  = string(ivl_expr_bits(const_expr), (size_t)ivl_expr_width(const_expr));
    reverse(basename.begin(), basename.end());
    string fullname  = string("const_") + to_string(num_constants) + string(".") + basename;

    return fullname;
}

unsigned int get_signal_msb(ivl_signal_t signal) {
    if (ivl_signal_packed_dimensions(signal) > 0) {
        // Check MSB is not negative
        assert((ivl_signal_packed_msb(signal, 0) >= 0) && \
            "NOT-SUPPORTED: negative MSB index.\n");
        
        return ivl_signal_packed_msb(signal, 0);
    } else {
        return 0;
    }
}

unsigned int get_signal_lsb(ivl_signal_t signal) {
    if (ivl_signal_packed_dimensions(signal) > 0) {
        // Check LSB is not negative
        assert((ivl_signal_packed_lsb(signal, 0) >= 0) && \
            "NOT-SUPPORTED: negative LSB index.\n");

        return ivl_signal_packed_lsb(signal, 0);
    } else {
        return 0;
    }
}

unsigned int get_const_msb(ivl_net_const_t constant) {
    return ivl_const_width(constant) - 1;
}

unsigned int get_expr_msb(ivl_expr_t expression) {
    return ivl_expr_width(expression) - 1;
}

// ----------------------------------------------------------------------------------
// ------------------------ Dot Graph Helper Functions ------------------------------
// ----------------------------------------------------------------------------------

// ----------------------------- IVL Signal Processing ------------------------------
string get_signal_node_label(ivl_signal_t signal) {
    stringstream ss;

    ss << "[";
    ss << get_signal_msb(signal);
    ss << ":";
    ss << get_signal_lsb(signal);
    ss << "]";

    return ss.str();
}

string get_signal_connection_label(ivl_signal_t source_signal, 
                                   ivl_signal_t sink_signal) {

    stringstream ss;
    
    ss << get_signal_node_label(source_signal);
    ss << "->";
    ss << get_signal_node_label(sink_signal);

    return ss.str();
}

string get_sliced_signal_connection_label(ivl_signal_t source_signal, 
                                          ivl_signal_t sink_signal, 
                                          node_slice_t signal_slice) {
    unsigned int sink_msb;
    unsigned int sink_lsb;
    unsigned int source_msb;
    unsigned int source_lsb;
    stringstream ss;

    if (signal_slice.type == SINK) {
        // sliced sink node (signal)
        sink_msb   = signal_slice.msb;
        sink_lsb   = signal_slice.lsb;
        source_msb = get_signal_msb(source_signal);
        source_lsb = get_signal_lsb(source_signal);
    } else {
        // sliced source node (signal)
        sink_msb = get_signal_msb(sink_signal);
        sink_lsb = get_signal_lsb(sink_signal);
        source_msb = signal_slice.msb;
        source_lsb = signal_slice.lsb;
    }
    
    ss << "[";
    ss << source_msb;
    ss << ":";
    ss << source_lsb;
    ss << "]->[";
    ss << sink_msb;
    ss << ":";
    ss << sink_lsb;
    ss << "]";

    return ss.str();
}

// ---------------------------- IVL Constant Processing -----------------------------
string get_const_node_label(ivl_net_const_t constant) {
    stringstream ss;

    ss << "[";
    ss << get_const_msb(constant);
    ss << ":0]";

    return ss.str();
}

string get_const_connection_label(ivl_net_const_t source_constant, 
                                  ivl_signal_t    sink_signal) {

    stringstream ss;
    
    ss << get_const_node_label(source_constant);
    ss << "->";
    ss << get_signal_node_label(sink_signal);

    return ss.str();
}

string get_sliced_const_connection_label(ivl_net_const_t source_constant,  
                                         node_slice_t    signal_slice) {

    unsigned int sink_msb;
    unsigned int sink_lsb;
    stringstream ss;

    // Source (const expr) node CANNOT be sliced
    assert(signal_slice.type == SINK && 
        "ERROR: slice source constant encountered.\n"); 

    // sliced sink node (signal)
    sink_msb = signal_slice.msb;
    sink_lsb = signal_slice.lsb;
    
    ss << get_const_node_label(source_constant);
    ss << "->[";
    ss << sink_msb;
    ss << ":";
    ss << sink_lsb;
    ss << "]";

    return ss.str();
}

// --------------------- IVL Constant Expression Processing -------------------------
string get_const_expr_node_label(ivl_expr_t const_expr) {
    stringstream ss;

    ss << "[";
    ss << get_expr_msb(const_expr);
    ss << ":0]";

    return ss.str();
}

string get_const_expr_connection_label(ivl_expr_t   source_const_expr, 
                                       ivl_signal_t sink_signal) {

    stringstream ss;
    
    ss << get_const_expr_node_label(source_const_expr);
    ss << "->";
    ss << get_signal_node_label(sink_signal);

    return ss.str();
}

string get_sliced_const_expr_connection_label(ivl_expr_t   source_const_expr, 
                                              node_slice_t signal_slice) {

    unsigned int sink_msb;
    unsigned int sink_lsb;
    stringstream ss;

    // Source (const expr) node CANNOT be sliced
    assert(signal_slice.type == SINK && 
        "ERROR: slice source constant encountered.\n"); 

    // sliced sink node (signal)
    sink_msb = signal_slice.msb;
    sink_lsb = signal_slice.lsb;
    
    ss << get_const_expr_node_label(source_const_expr);
    ss << "->[";
    ss << sink_msb;
    ss << ":";
    ss << sink_lsb;
    ss << "]";

    return ss.str();
}

// ----------------------------------------------------------------------------------
// ---------------------- Connection Enumeration Functions --------------------------
// ----------------------------------------------------------------------------------
void find_combinational_connections(SignalGraph* sg) {
    // Get signals adjacency list
    sig_map_t signals_map = sg->get_signals_map();

    // Create a signals map iterator
    sig_map_t::iterator it = signals_map.begin();
 
    // Iterate over all signals in adjacency list
    while (it != signals_map.end()) {  
        ivl_signal_t sink_signal = it->first;

        // Print signal name -- signal dimensions
        fprintf(stdout, "%s:\n", get_signal_fullname(sink_signal).c_str());

        // Get signal nexus
        // There is exactly one nexus for each WORD of a signal.
        // Since we only support non-arrayed signals (above), 
        // each signal only has one nexus.
        const ivl_nexus_t sink_nexus = ivl_signal_nex(sink_signal, 0);

        // Check Nexus IS NOT NULL
        assert(sink_nexus);

        // Propagate the nexus
        propagate_nexus(sink_nexus, sink_signal, sg, "  ");

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

// ----------------------------------------------------------------------------------
// ------------------------ IVL Target Entry Point "main" ---------------------------
// ----------------------------------------------------------------------------------
int target_design(ivl_design_t design) {
    ivl_scope_t*   roots     = 0; // root scopes of the design
    unsigned       num_roots = 0; // number of root scopes of the design
    Reporter       reporter;      // reporter object (prints messages)
    SignalGraph    sg;            // signal graph object

    // Initialize reporter checking objects
    reporter = Reporter();
    reporter.init(LAUNCH_MESSAGE);

    // Initialize SignalGraph
    sg = SignalGraph(ivl_design_flag(design, "-o"));
    
    // Get root scopes (top level modules) of design
    reporter.print_message(SCOPE_EXPANSION_MESSAGE);
    ivl_design_roots(design, &roots, &num_roots);
    Error::check_scope_types(roots, num_roots);
    reporter.root_scopes(roots, num_roots);

    // Find all critical signals and dependencies in the design
    reporter.print_message(SIGNAL_ENUM_MESSAGE);
    sg.find_all_signals(roots, num_roots);
    reporter.num_signals(sg.get_num_signals());
    reporter.signal_names(sg.get_signals_map());

    // Find signal-to-signal connections
    reporter.print_message(COMB_CONNECTION_ENUM_MESSAGE);
    find_combinational_connections(&sg);      // Process COMBINATIONAL logic connections
    reporter.print_message(BEHAVE_CONNECTION_ENUM_MESSAGE);
    find_behavioral_connections(design, &sg); // Process BEHAVIORAL logic connections

    // Report Graph Stats
    reporter.line_separator();
    reporter.num_signals(sg.get_num_signals());
    reporter.num_constants(sg.get_num_constants());
    reporter.num_connections(sg.get_num_connections());

    // Save dot graph to file
    sg.save_dot_graph();

    // Report total execution time
    reporter.end();

    return 0;
}
