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

#ifndef TGT_DFG_INCLUDE_SIGNAL_GRAPH_H_
#define TGT_DFG_INCLUDE_SIGNAL_GRAPH_H_

#include <string>

#include "iverilog/ivl_target.h"
#include "tgt-dfg/include/connection.h"
#include "tgt-dfg/include/dot_graph.h"
#include "tgt-dfg/include/signal.h"

class SignalGraph {
 public:
  // Constructors
  SignalGraph();
  explicit SignalGraph(cmd_args_map_t* cmd_args);

  // Destructors
  ~SignalGraph();

  // Signal Enumeration
  void find_all_signals(ivl_scope_t* scopes, unsigned int num_scopes);
  bool check_if_ignore_signal(ivl_signal_t signal) const;
  bool check_if_ignore_signal(Signal* signal) const;
  bool check_if_ignore_mem_signal(ivl_signal_t mem_signal) const;

  // Connection Enumeration
  conn_q_t* get_connections(Signal* sink_signal);

  bool check_if_connection_exists(Signal* sink_signal, Connection* new_conn);

  bool check_if_local_connection_exists(Signal* sink_signal,
                                        Connection* new_conn);

  bool add_connection(Signal* sink_signal, Signal* source_signal,
                      signal_slice_t sink_slice, signal_slice_t source_slice,
                      std::string ws);

  void track_local_signal_connection(Signal* sink_signal, Signal* source_signal,
                                     signal_slice_t sink_slice,
                                     signal_slice_t source_slice,
                                     std::string ws);

  void process_local_connections(std::string ws);

  // Stats Counters
  uint64_t get_num_connections() const;
  uint64_t get_num_signals() const;
  uint64_t get_num_mem_signals() const;
  uint64_t get_num_local_signals() const;
  uint64_t get_num_constants() const;

  // Signal Map
  sig_map_t get_signals_map() const;
  bool in_signals_map(Signal* signal) const;
  Signal* get_signal_from_ivl_signal(ivl_signal_t ivl_signal);

  // Dot Graph Management
  void write_signals_to_dot_graph();
  void save_dot_graph();

 private:
  // Stats Counters
  uint64_t num_signals_;            // number of signals enumerated in design
  uint64_t num_signals_ignored_;    // number of signals ignored
  uint64_t num_mem_signals_;        // number of signals ignored
  uint64_t num_local_signals_;      // number of local signals optimized away
  uint64_t num_constants_;          // number of constants in design
  uint64_t num_connections_;        // number of connections in design
  uint64_t num_local_connections_;  // number of local connections to process

  // Configs
  unsigned int mem_signal_size_;  // threshold for classifying a memory signal

  // Graph Data
  DotGraph* dg_;                      // dot graph object
  sig_map_t signals_map_;             // IVL signal to Signal object map
  consts_map_t consts_map_;           // Const ID to Signal object map
  conn_map_t connections_map_;        // sink signal to connections map
  conn_map_t local_connections_map_;  // local signal connections to process
  string_map_t signals_to_ignore_;    // names of signals to ignore (user input)
  sig_set_t mem_signals_to_ignore_;   // pointers of memory signals to ignore

  // Signal Enumeration
  void find_signals(ivl_scope_t scope);
  void add_signal(ivl_signal_t signal);

  // Config Loading
  void process_cmd_line_args(cmd_args_map_t* cmd_args);
  void load_signals_to_ignore(std::string file_path);
  void add_signal_to_ignore(std::string signal_basename);

  // Destructor Helpers
  void delete_signals_map();
  void delete_constants_map();
  void delete_connections_queue(conn_q_t* conn_q);
  void delete_connections_map();
};

#endif  // TGT_DFG_INCLUDE_SIGNAL_GRAPH_H_
