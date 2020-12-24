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

#include "tgt-dfg/include/reporter.h"

#include <cassert>
#include <cstring>
#include <ctime>
#include <string>

#include "tgt-dfg/include/dfg.h"
#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/signal.h"
#include "tgt-dfg/include/signal_graph.h"

// Constructors
Reporter::Reporter() : file_path_(NULL), file_ptr_(NULL) { init(); }

Reporter::Reporter(FILE* file_ptr) : file_path_(NULL), file_ptr_(file_ptr) {
  init();
}

Reporter::Reporter(const char* p) {
  file_ptr_ = NULL;
  set_file_path(p);
  init();
}

// Destructors
Reporter::~Reporter() {
  DEBUG_DESTRUCTORS(fprintf(DESTRUCTOR_PRINTS_FILE_PTR,
                            "Executing Reporter destructor...\n");)
  if (file_ptr_ && (file_ptr_ != stdout)) {
    close_file();
  }
  if (start_times_q_) {
    delete (start_times_q_);
    start_times_q_ = NULL;
  }
}

// Getters
const char* Reporter::get_file_path() const { return file_path_; }

// Setters
void Reporter::set_file_path(const char* p) { file_path_ = p; }

// Task Tracking
void Reporter::start_task(const char* message) {
  // Print starting message
  print_message(message);

  // Check that not more than two timers on the queue
  assert(start_times_q_->size() < 2 &&
         "ERROR: cannot track more concurrent task times.\n");

  // Start timer and push to queue
  start_times_q_->push_back(clock());
}

void Reporter::end_task() {
  double start_time = 0;
  double execution_time = 0;

  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");

  // Compute execution time
  start_time = start_times_q_->back();
  start_times_q_->pop_back();
  execution_time = (clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC);
  fprintf(file_ptr_, "Execution Time: %f (s)\n", execution_time);
}

// Message Printing
void Reporter::print_message(const char* message) const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  line_separator();
  fprintf(file_ptr_, "%s\n", message);
}

void Reporter::line_separator() const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  fprintf(file_ptr_, "%s\n", LINE_SEPARATOR);
}

void Reporter::configurations(cmd_args_map_t* cmd_args) const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  fprintf(file_ptr_, "Output DOT graph filname: %s\n",
          cmd_args->at(OUTPUT_FILENAME_FLAG).c_str());
  fprintf(file_ptr_, "Clock signal basename:    %s\n",
          cmd_args->at(CLK_BASENAME_FLAG).c_str());
}

void Reporter::root_scopes(ivl_scope_t* scopes, unsigned int num_scopes) const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  fprintf(file_ptr_, "Found %d top-level module(s):\n", num_scopes);
  std::string scope_name = "UNKONWN";
  for (unsigned int i = 0; i < num_scopes; i++) {
    scope_name = ivl_scope_name(scopes[i]);
    fprintf(file_ptr_, "    %s\n", scope_name.c_str());
  }
}

void Reporter::num_signals(uint64_t num_sigs) const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  fprintf(file_ptr_, "Number of signals found: %lu\n", num_sigs);
}

void Reporter::signal_names(sig_map_t signals_map) const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  fprintf(file_ptr_, "Signal Names:\n");

  // Iterate over the map until the end
  sig_map_t::iterator it = signals_map.begin();
  while (it != signals_map.end()) {
    fprintf(file_ptr_, "    %s\n", it->second->get_fullname().c_str());
    it++;
  }
}

void Reporter::graph_stats(SignalGraph* sg) const {
  assert(file_ptr_ != NULL && "ERROR: reporter file ptr is NULL.\n");
  fprintf(file_ptr_, "Number of signals found:          %lu\n",
          sg->get_num_signals());
  fprintf(file_ptr_, "Number of memory signals skipped: %lu\n",
          sg->get_num_mem_signals());
  fprintf(file_ptr_, "Number of local signals removed:  %lu\n",
          sg->get_num_local_signals());
  fprintf(file_ptr_, "Number of constants found:        %lu\n",
          sg->get_num_constants());
  fprintf(file_ptr_, "Number of connections found:      %lu\n",
          sg->get_num_connections());
}

// Other
void Reporter::init() {
  if (!file_path_ && !file_ptr_) {
    file_ptr_ = stdout;
  } else if (!file_ptr_) {
    open_file();
  }
  start_times_q_ = new times_q_t();
}

void Reporter::open_file() {
  file_ptr_ = fopen(file_path_, "w");
  if (!file_ptr_) {
    fprintf(stderr, "ERROR: Could not open file %s\n",
            file_path_ ? file_path_ : "stdout");
    exit(-1);
  }
}

void Reporter::close_file() {
  if (file_ptr_ != stdout) {
    fclose(file_ptr_);
  }
  file_ptr_ = NULL;
}
