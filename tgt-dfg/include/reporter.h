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

#ifndef TGT_DFG_INCLUDE_REPORTER_H_
#define TGT_DFG_INCLUDE_REPORTER_H_

#include <cstdio>
#include <vector>

#include "iverilog/ivl_target.h"
#include "tgt-dfg/include/signal.h"
#include "tgt-dfg/include/signal_graph.h"

typedef std::vector<clock_t> times_q_t;

class Reporter {
 public:
  // Constructors
  Reporter();
  explicit Reporter(FILE* file_ptr);
  explicit Reporter(const char* p);

  // Destructors
  ~Reporter();

  // Getters
  const char* get_file_path() const;

  // Setters
  void set_file_path(const char* p);

  // Task Time Tracking
  void start_task(const char* message);
  void end_task();

  // Message Printing
  void print_message(const char* message) const;
  void line_separator() const;
  void configurations(cmd_args_map_t* cmd_args) const;
  void root_scopes(ivl_scope_t* scopes, unsigned int num_scopes) const;
  void num_signals(uint64_t num_sigs) const;
  void graph_stats(SignalGraph* sg) const;
  void signal_names(sig_map_t signals_map) const;

 private:
  const char* file_path_;
  FILE* file_ptr_;
  times_q_t* start_times_q_;

  // File Operations
  void open_file();
  void close_file();

  // Other
  void init();
};

#endif  // TGT_DFG_INCLUDE_REPORTER_H_
