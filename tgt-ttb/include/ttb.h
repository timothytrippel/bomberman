/*
File:        ttb.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_HEADER__
#define __TTB_HEADER__

// Standard Headers
#include <string>

// IVL API Header
#include  <ivl_target.h>

// TTB Headers
#include "ttb_typedefs.h"
#include "dot_graph.h"

using namespace std;

class SignalGraph {
    public:
        // Constructors
        SignalGraph();
        SignalGraph(const char* dot_graph_fname);

        // Signal Enumeration
        void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);

        // Connection Enumeration
        void find_all_connections();

        // Getters
        unsigned long get_num_connections();
        unsigned long get_num_signals();
        sig_map_t get_signals_map();
        void save_dot_graph();

        // Helper Functions
        static string       get_signal_fullname(ivl_signal_t signal);
        static string       get_constant_fullname(ivl_net_const_t constant);
        static string       get_connection_label();
        static unsigned int get_signal_msb(ivl_signal_t signal);
        static unsigned int get_signal_lsb(ivl_signal_t signal);
        static string       get_signal_node_label(ivl_signal_t signal);

        static string       get_signal_connection_label(ivl_signal_t source_signal, 
                                                        ivl_signal_t sink_signal);

        static string       get_signal_connection_label(ivl_signal_t source_signal, 
                                                        ivl_signal_t sink_signal, 
                                                        SliceInfo    signal_slice);
    private:
        // (Private) Member Variables
        unsigned long     num_connections_; // number of connections enumerated in design
        sig_map_t         signals_map_;     // signal graph (adjacency list)
        DotGraph          dg_;              // dot graph object
        vector<SliceInfo> signal_slices_;   // signal slice information stack
        
        // Signal Enumeration
        void find_signals(ivl_scope_t scope);

        // Connection Enumeration
        void add_constant_connection(ivl_signal_t    sink_signal, 
                                     ivl_net_const_t source_constant,
                                     string          ws);
        
        void add_connection(ivl_signal_t sink_signal, 
                            ivl_signal_t source_signal,  
                            string       ws);

        void track_lpm_connection_slice(unsigned int      msb, 
                                        unsigned int      lsb,
                                        slice_node_type_t node_type);

        void propagate_nexus(ivl_nexus_t  nexus, 
                             ivl_signal_t sink_signal, 
                             string       ws);

        void propagate_logic(ivl_net_logic_t logic, 
                             ivl_nexus_t     sink_nexus, 
                             ivl_signal_t    sink_signal, 
                             string          ws);

        void propagate_lpm(ivl_lpm_t    lpm, 
                           ivl_nexus_t  sink_nexus, 
                           ivl_signal_t sink_signal, 
                           string       ws);

        void process_lpm_part_select(ivl_lpm_t    lpm, 
                                     ivl_signal_t sink_signal, 
                                     string       ws);

        void process_lpm_concat(ivl_lpm_t    lpm, 
                                ivl_signal_t sink_signal, 
                                string       ws);

        void process_lpm_mux(ivl_lpm_t    lpm, 
                             ivl_signal_t sink_signal, 
                             string       ws);

        void process_lpm_basic(ivl_lpm_t    lpm, 
                               ivl_signal_t sink_signal, 
                               string       ws);

        void propagate_constant(ivl_net_const_t constant,
                                ivl_nexus_t  sink_nexus, 
                                ivl_signal_t sink_signal, 
                                string       ws);

        // void process_constant(ivl_net_const_t constant,
        //                            ivl_nexus_t  sink_nexus, 
        //                            ivl_signal_t sink_signal, 
        //                            string       ws);

        // Helper Functions
        const char* get_lpm_type_as_string(ivl_lpm_t lpm);
        const char* get_logic_type_as_string(ivl_net_logic_t logic);
        const char* get_const_type_as_string(ivl_net_const_t constant);
};  

#endif
