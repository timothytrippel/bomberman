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
    ivl_signal_t    temp_sig          = NULL;
    ivl_net_const_t temp_const        = NULL;
    ivl_signal_t    source_ivl_signal = NULL;

    // Sensitivity list in HDL
    vector<string> signals_list;

    // Source signal counter
    unsigned int   num_possible_signals = 0;

    // fprintf(DEBUG_PRINTS_FILE_PTR, 
    //     "%sfound %d event nexus ptrs\n", ws.c_str(), ivl_nexus_ptrs(nexus));

    // Get line of source code file
    string source_code_line = Tracker::get_file_line(statement);

    // Get event nexus pointer IVL signal (source signal)
    if (ivl_nexus_ptrs(nexus) == 1) {

        // IVL source signal is at index 0
        source_ivl_signal = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, 0));

    } else if (ivl_nexus_ptrs(nexus) > 1) {

        // Find IVL source signal whose scope matches that of the event itself
        for (unsigned int i = 0; i < ivl_nexus_ptrs(nexus); i++) {

            // Check nexus pointer type(s) are signal object(s) only.
            // Other nexus types (i.e. LOGs and LPMs) are just connections 
            // to the event signal which are propagated during the continuous
            // logic propagation phase.
            if ((temp_sig = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, i)))) {

                fprintf(DEBUG_PRINTS_FILE_PTR, "%spotential event nexus signal: %s.%s\n", 
                    ws.c_str(), 
                    ivl_scope_name(ivl_signal_scope(temp_sig)),
                    ivl_signal_basename(temp_sig));

                // Check if scope of source signal matches scope of event
                if (ivl_signal_scope(temp_sig) == ivl_event_scope(event) &&
                    !ivl_signal_local(temp_sig)) {

                    // Check if source signal already found
                    if (!source_ivl_signal) {

                        fprintf(DEBUG_PRINTS_FILE_PTR, "%sfound event nexus signal: %s.%s\n", 
                            ws.c_str(), 
                            ivl_scope_name(ivl_signal_scope(temp_sig)),
                            ivl_signal_basename(temp_sig));

                        // Set source signal
                        source_ivl_signal = temp_sig;

                    } else {

                        // Convert signal basenames to string
                        string source_sig_basename = string(ivl_signal_basename(source_ivl_signal));
                        string temp_sig_basename   = string(ivl_signal_basename(temp_sig));

                        // Check if basename in source code line
                        if (source_code_line.find(temp_sig_basename) == string::npos) {
                            
                            // if its not, the first signal was the right one
                            continue;

                        } else {

                            // check if the first signal assigned as the source signal was wrong
                            if (source_code_line.find(source_sig_basename) == string::npos) { 
                                
                                source_ivl_signal = temp_sig;

                            } else {

                                fprintf(DEBUG_PRINTS_FILE_PTR, "%sHDL code line: %s\n", 
                                    ws.c_str(), 
                                    source_code_line.c_str());

                                // Throw Error
                                Error::multiple_valid_event_nexus_ptrs(statement);
                            }
                        }
                    }
                }
            } else if ((temp_const = ivl_nexus_ptr_con(ivl_nexus_ptr(nexus, i)))) {

                // Check if scope of source (constant) signal matches scope of event
                if (ivl_const_scope(temp_const) == ivl_event_scope(event)) {
                    
                    // Get bitstring
                    string bitstring = Signal::get_const_bitstring(ivl_nexus_ptr_con(ivl_nexus_ptr(nexus, i)));

                    fprintf(DEBUG_PRINTS_FILE_PTR, "%sevent nexus signal: %s.%s\n", 
                        ws.c_str(), 
                        ivl_scope_name(ivl_const_scope(ivl_nexus_ptr_con(ivl_nexus_ptr(nexus, i)))),
                        bitstring.c_str());

                    // Throw Warning
                    Error::constant_event_nexus_ptr_warning(statement);
                }
            }
        } 
    } else {

        // Throw Error
        Error::zero_event_nexus_ptrs(statement);
    }

    // (if it was not, could be inside a test bench scope where signals are declared with full hierarchical path)
    if (!source_ivl_signal) {

        // Get sensitivity list (strings of signal names)
        signals_list = Tracker::get_event_sensitivity_list(source_code_line);

        // Convert list of fullnames to list of basename
        signals_list = Tracker::convert_fullnames_to_basenames(signals_list);
        
        // fprintf(stdout, "*********\n");
        // Tracker::print_string_list(signals_list);
        // fprintf(stdout, "*********\n");

        // Re-iterate over event nexus pointers
        for (unsigned int i = 0; i < ivl_nexus_ptrs(nexus); i++) {

            // Only look at signals
            if ((temp_sig = ivl_nexus_ptr_sig(ivl_nexus_ptr(nexus, i)))) {

                // Check if basename in sensitivity list
                if(find(signals_list.begin(), 
                        signals_list.end(), 
                        string(ivl_signal_basename(temp_sig))) 
                    != signals_list.end()) {

                    source_ivl_signal = temp_sig;
                    num_possible_signals++;
                }
            }
        }
    }

    // Error out if more than one possible event signal
    if (num_possible_signals > 1) {
        Error::multiple_valid_event_nexus_ptrs(statement);
    }

    // Check that a source signal was found
    if (!source_ivl_signal) {

        // Print event scope
        fprintf(stderr, "Event Scope: %s\n", ivl_scope_name(ivl_event_scope(event)));

        // Print HDL code, file, and line number of event
        Tracker::print_statement_hdl(statement, ws);

        // Throw Warning
        Error::unkown_event_source_signal_warning(statement);

        return 0;
    }

    // Check if CLK signal is one of source signals. If so, 
    // it means we have entered an always block, and subsequent
    // sink signals should be marked as FFs.
    if (check_if_clk_signal(source_ivl_signal)) {
        fprintf(DEBUG_PRINTS_FILE_PTR, "%sprocess is clocked\n", ws.c_str());
        set_inside_ff_block();
    } 
    
    // Check if signal is to be ignored
    if (!sg_->check_if_ignore_mem_signal(source_ivl_signal)) {

        // Get signal object from IVL source signal
        Signal* source_signal = sg_->get_signal_from_ivl_signal(source_ivl_signal);

        // Push signal to source signals queue
        push_source_signal(source_signal, 0, ws + WS_TAB);

        return 1;
    } else {
        return 0;    
    }
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
