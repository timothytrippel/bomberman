/*
 * Copyright © 2019, Massachusetts Institute of Technology
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TGT_DFG_INCLUDE_TRACKER_H_
#define TGT_DFG_INCLUDE_TRACKER_H_

#include <set>
#include <string>
#include <vector>

#include "iverilog/ivl_target.h"
#include "tgt-dfg/include/connection.h"
#include "tgt-dfg/include/signal.h"
#include "tgt-dfg/include/signal_graph.h"
#include "tgt-dfg/include/signal_q.h"

class Tracker {
 public:
  // Constructors
  Tracker();
  Tracker(cmd_args_map_t* cmd_args, SignalGraph* sg);

  // Destructors
  ~Tracker();

  // Flip-Flop Status Tracking
  void set_inside_ff_block();
  void clear_inside_ff_block();
  bool check_if_inside_ff_block() const;
  bool check_if_clk_signal(ivl_signal_t source_signal) const;

  // Source Signal Tracking
  Signal* get_source_signal(unsigned int index) const;
  Signal* pop_source_signal(std::string ws);
  void pop_source_signals(unsigned int num_signals, std::string ws);
  void push_source_signal(Signal* source_signal, unsigned int id,
                          std::string ws);

  // Slice Tracking
  void enable_slicing();
  void disable_slicing();
  void set_source_slice(Signal* signal, unsigned int msb, unsigned int lsb,
                        std::string ws);
  void set_sink_slice(Signal* signal, unsigned int msb, unsigned int lsb,
                      std::string ws);
  void set_source_slice(Signal* signal, signal_slice_t slice, std::string ws);
  void set_sink_slice(Signal* signal, signal_slice_t slice, std::string ws);
  void shift_source_slice(Signal* signal, int num_bits, std::string ws);
  void shift_sink_slice(Signal* signal, int num_bits, std::string ws);

  // Depth Tracking (tracks number of signals at a given depth)
  unsigned int pop_scope_depth();
  void push_scope_depth(unsigned int num_signals);

  // Continuous HDL Processing
  void find_continuous_connections();
  void propagate_nexus(ivl_nexus_t nexus, Signal* sink_signal, std::string ws);
  bool propagate_signal(ivl_signal_t ivl_source_signal, Signal* sink_signal,
                        std::string ws);
  bool is_sig1_parent_of_sig2(ivl_signal_t signal_1, ivl_signal_t signal_2);
  bool in_same_scope(ivl_signal_t signal_1, ivl_signal_t signal_2);
  bool process_signal_connect(ivl_signal_t ivl_source_signal,
                              Signal* sink_signal, std::string ws);
  bool process_signal_case(unsigned int case_num,
                           ivl_signal_t ivl_source_signal, Signal* sink_signal,
                           std::string ws);
  void propagate_logic(ivl_net_logic_t logic, ivl_nexus_t sink_nexus,
                       Signal* sink_signal, std::string ws);
  void propagate_lpm(ivl_lpm_t lpm, Signal* sink_signal, std::string ws);
  void process_lpm_basic(ivl_lpm_t lpm, Signal* sink_signal, std::string ws);
  void process_lpm_part_select(ivl_lpm_t lpm, Signal* sink_signal,
                               std::string ws);
  void process_lpm_concat(ivl_lpm_t lpm, Signal* sink_signal, std::string ws);
  void process_lpm_mux(ivl_lpm_t lpm, Signal* sink_signal, std::string ws);
  void process_lpm_memory(ivl_lpm_t lpm, Signal* sink_signal, std::string ws);
  void propagate_constant(ivl_net_const_t constant, Signal* sink_signal,
                          std::string ws);

  // Procedural HDL Processing
  int process_process(ivl_process_t process);
  unsigned int process_expression(ivl_expr_t expression,
                                  ivl_statement_t statement, std::string ws);
  std::vector<unsigned int> process_array_index_expression(
      ivl_signal_t base_ivl_signal, ivl_expr_t expression,
      ivl_statement_t statement, std::string ws);
  int process_index_expression(ivl_expr_t expression, ivl_statement_t statement,
                               std::string ws);
  unsigned int process_expression_signal(ivl_expr_t expression,
                                         ivl_statement_t statement,
                                         std::string ws);
  unsigned int process_expression_number(ivl_expr_t expression, std::string ws);
  unsigned int process_expression_select(ivl_expr_t expression,
                                         ivl_statement_t statement,
                                         std::string ws);
  unsigned int process_expression_concat(ivl_expr_t expression,
                                         ivl_statement_t statement,
                                         std::string ws);
  unsigned int process_expression_unary(ivl_expr_t expression,
                                        ivl_statement_t statement,
                                        std::string ws);
  unsigned int process_expression_binary(ivl_expr_t expression,
                                         ivl_statement_t statement,
                                         std::string ws);
  unsigned int process_expression_ternary(ivl_expr_t expression,
                                          ivl_statement_t statement,
                                          std::string ws);
  unsigned int process_event(ivl_event_t event, ivl_statement_t statement,
                             std::string ws);
  unsigned int process_event_nexus(ivl_event_t event, ivl_nexus_t nexus,
                                   ivl_statement_t statement, std::string ws);
  void process_statement(ivl_statement_t statement, std::string ws);
  void process_statement_wait(ivl_statement_t statement, std::string ws);
  void process_statement_condit(ivl_statement_t statement, std::string ws);
  void process_statement_assign_rval(Signal* sink_signal,
                                     ivl_statement_t statement, std::string ws);
  void process_statement_assign_lval(ivl_statement_t statement, std::string ws);
  void process_statement_assign(ivl_statement_t statement, std::string ws);
  void process_statement_block(ivl_statement_t statement, std::string ws);
  void process_statement_case(ivl_statement_t statement, std::string ws);
  void process_statement_delay(ivl_statement_t statement, std::string ws);

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
  bool ignore_constants_;  // indicates if constants should be ignored

  // Config Data
  std::string clk_basename_;  // indicates if processing inside a FF block

  // State Tracking Flags
  bool inside_ff_block_;        // indicates if processing inside a FF block
  bool slicing_enabled_;        // indicates if processing bit slices
  bool array_index_is_signal_;  // indicates if array index is a signal (1) or
                                // constant (0)

  // State Tracking Data
  SignalGraph* sg_;         // signal graph object
  SignalQ source_signals_;  // source signals queue
  std::set<ivl_nexus_t>
      explored_nexi_;  // used to detect loops in continuous logic
  std::vector<unsigned int>
      num_signals_at_depth_;  // tracks num source nodes at current depth

  // Config Loading
  void process_cmd_line_args(cmd_args_map_t* cmd_args);

  // Other
  static void print_string_list(std::vector<std::string> str_list);
  static void print_statement_hdl(ivl_statement_t statement, std::string ws);
  static std::string get_file_line(ivl_statement_t statement);
  static std::string get_event_signal_basename(std::string fullname);
  static std::vector<std::string> tokenize_string(std::string s,
                                                  char delimeter);
  static std::vector<std::string> get_event_sensitivity_list(
      std::string hdl_code_line);
  static std::vector<std::string> remove_string_from_list(
      std::vector<std::string> str_list, std::string str2remove);
  static std::vector<std::string> convert_fullnames_to_basenames(
      std::vector<std::string> signals_list);
};

#endif  // TGT_DFG_INCLUDE_TRACKER_H_
