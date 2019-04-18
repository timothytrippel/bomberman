/*
File:        tracker.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TRACKER_HEADER__
#define __TRACKER_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <string>
#include <vector>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "signal.h"
#include "connection.h"
#include "signal_q.h"
#include "signal_graph.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------ Tracker ---------------------------
// ------------------------------------------------------------

class Tracker {
	public:
		// Constructors
		Tracker();
		Tracker(
			cmd_args_map_t* cmd_args, 
			SignalGraph*    sg);

		// Destructors
		~Tracker();

		// Flip-Flop Status Tracking
		void set_inside_ff_block();
        void clear_inside_ff_block();
        bool check_if_inside_ff_block() const;
        bool check_if_clk_signal(ivl_signal_t source_signal) const;

		// Source Signal Tracking
		Signal* get_source_signal(unsigned int index) const;
		Signal* pop_source_signal(string ws);
		void    pop_source_signals(unsigned int num_signals, string ws);

		void push_source_signal(
			Signal*      source_signal, 
			unsigned int id,
			string       ws);

		// Slice Tracking
		void enable_slicing();
		void disable_slicing();

		void set_source_slice(
			Signal*      signal, 
			unsigned int msb, 
			unsigned int lsb, 
			string       ws);

		void set_sink_slice(
			Signal*      signal, 
			unsigned int msb, 
			unsigned int lsb, 
			string       ws);

		void set_source_slice(
			Signal*        signal, 
			signal_slice_t slice, 
			string         ws);

		void set_sink_slice(
			Signal*        signal, 
			signal_slice_t slice, 
			string         ws);

		void update_source_slice(
			Signal*      signal, 
			unsigned int msb, 
			unsigned int lsb, 
			string       ws);

		void update_sink_slice(
			Signal*      signal, 
			unsigned int msb, 
			unsigned int lsb, 
			string       ws);

		void update_source_slice(
			Signal*        signal, 
			signal_slice_t slice, 
			string         ws);

		void update_sink_slice(
			Signal*        signal, 
			signal_slice_t slice, 
			string         ws);

		// Depth Tracking (tracks number of signals at a given depth)
        unsigned int pop_scope_depth();
        void         push_scope_depth(unsigned int num_signals);

        // Continuous HDL Processing
		void find_continuous_connections();

		// Continuous HDL Processing - Nexus
		void propagate_nexus(
		    ivl_nexus_t nexus,
		    Signal*     sink_signal,
		    string      ws);

		// Continuous HDL Processing - Signal
		bool propagate_signal(
		    ivl_signal_t ivl_source_signal,
		    Signal*      sink_signal,
		    string       ws);

		bool is_sig1_parent_of_sig2(
		    ivl_signal_t signal_1, 
		    ivl_signal_t signal_2);

		bool in_same_scope(
		    ivl_signal_t signal_1, 
		    ivl_signal_t signal_2);

		bool process_signal_connect(
		    ivl_signal_t ivl_source_signal,
		    Signal*      sink_signal,
		    string       ws);

		bool process_signal_case(
		    unsigned int case_num,
		    ivl_signal_t ivl_source_signal,
		    Signal*      sink_signal,
		    string       ws);

		// Continuous HDL Processing - Logic
		void propagate_logic(
		    ivl_net_logic_t logic,
		    ivl_nexus_t     sink_nexus,
		    Signal*         sink_signal,
		    string          ws);

		// Continuous HDL Processing - LPM
		void propagate_lpm(
		    ivl_lpm_t lpm,
		    Signal*   sink_signal,
		    string    ws);

		void process_lpm_basic(
		    ivl_lpm_t lpm,
		    Signal*   sink_signal,
		    string    ws);

		void process_lpm_part_select(
		    ivl_lpm_t lpm, 
		    Signal*   sink_signal,
		    string    ws);

		void process_lpm_concat(
		    ivl_lpm_t lpm,
		    Signal*   sink_signal,
		    string    ws);

		void process_lpm_mux(
		    ivl_lpm_t lpm,
		    Signal*   sink_signal,
		    string    ws);

		void process_lpm_memory(
		    ivl_lpm_t lpm,
		    Signal*   sink_signal,
		    string    ws);

		// Continuous HDL Processing - Constant
		void propagate_constant(
		    ivl_net_const_t constant,
		    Signal*         sink_signal, 
		    string          ws);

		// Procedural HDL Processing
		// Procedural HDL Processing - Process
		int process_process(ivl_process_t process);

		// Procedural HDL Processing - Expression
		unsigned int process_expression(
		    ivl_expr_t      expression,
		    ivl_statement_t statement,  
		    string          ws);

		unsigned int process_index_expression(
			ivl_expr_t      expression,
		    ivl_statement_t statement,  
		    string          ws);

		unsigned int process_expression_signal(
		    ivl_expr_t      expression,
		    ivl_statement_t statement,
		    string          ws);

		unsigned int process_expression_number(
		    ivl_expr_t expression, 
		    string     ws);

		unsigned int process_expression_select(
		    ivl_expr_t      expression, 
		    ivl_statement_t statement,
		    string          ws);

		unsigned int process_expression_concat(
		    ivl_expr_t      expression, 
		    ivl_statement_t statement,
		    string          ws);

		unsigned int process_expression_unary(
		    ivl_expr_t      expression, 
		    ivl_statement_t statement,
		    string          ws);

		unsigned int process_expression_binary(
		    ivl_expr_t      expression, 
		    ivl_statement_t statement,
		    string          ws);

		unsigned int process_expression_ternary(
		    ivl_expr_t      expression, 
		    ivl_statement_t statement,
		    string          ws);

		// Procedural HDL Processing - Event
		unsigned int process_event(
		    ivl_event_t     event, 
		    ivl_statement_t statement, 
		    string          ws);

		unsigned int process_event_nexus(
			ivl_event_t     event,
		    ivl_nexus_t     nexus, 
		    ivl_statement_t statement,  
		    string          ws);

		// Procedural HDL Processing - Statement
		void process_statement(
		    ivl_statement_t statement, 
		    string          ws);

		void process_statement_wait(
		    ivl_statement_t statement,  
		    string          ws);

		void process_statement_condit(
		    ivl_statement_t statement,  
		    string          ws);

		Signal* process_statement_assign_lval(
		    ivl_statement_t statement,
		    string          ws);

		void process_statement_assign(
		    ivl_statement_t statement,  
		    string          ws);

		void process_statement_block(
		    ivl_statement_t statement, 
		    string          ws);

		void process_statement_case(
		    ivl_statement_t statement, 
		    string          ws);

		void process_statement_delay(
		    ivl_statement_t statement,
		    string          ws);

		// Logging
		const char* get_signal_port_type_as_string(ivl_signal_t signal);
		const char* get_logic_type_as_string(ivl_net_logic_t logic);
		const char* get_lpm_type_as_string(ivl_lpm_t lpm);
		const char* get_const_type_as_string(ivl_net_const_t constant);
		const char* get_process_type_as_string(ivl_process_t process);
		const char* get_expr_type_as_string(ivl_expr_t expression);
		const char* get_statement_type_as_string(ivl_statement_t statement);

	private:
		// Config Flags
		bool ignore_constants_; // indicates if constants should be ignored

		// Config Data
		string clk_basename_; // indicates if processing inside a FF block

		// State Tracking Flags
        bool inside_ff_block_; // indicates if processing inside a FF block
        bool slicing_enabled_; // indicates if processing bit slices

        // State Tracking Data
        SignalGraph*         sg_;                   // signal graph object
        SignalQ              source_signals_;       // source signals queue
        vector<unsigned int> num_signals_at_depth_; // tracks num source nodes at current depth
        
        // Config Loading
        void process_cmd_line_args(cmd_args_map_t* cmd_args);
};

#endif
