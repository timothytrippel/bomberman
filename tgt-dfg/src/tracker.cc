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

#include "tgt-dfg/include/tracker.h"

#include <cassert>

#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/error.h"

// Constructors
Tracker::Tracker()
    : ignore_constants_(false),
      clk_basename_(""),
      inside_ff_block_(false),
      slicing_enabled_(false),
      array_index_is_signal_(false),
      sg_(NULL),
      source_signals_(),
      explored_nexi_(),
      num_signals_at_depth_() {}

Tracker::Tracker(cmd_args_map_t* cmd_args, SignalGraph* sg) {
  // Intialize Flags
  inside_ff_block_ = false;
  slicing_enabled_ = false;
  array_index_is_signal_ = false;
  ignore_constants_ = false;

  // Intialize Data
  clk_basename_ = std::string("");
  sg_ = sg;

  // Process command line args
  process_cmd_line_args(cmd_args);
}

// Destructors
Tracker::~Tracker() {
  // 1. Set pointer to SignalsGraph to NULL
  sg_ = NULL;

  // 2. Check that all explored signals processed
  assert(explored_nexi_.size() == 0 &&
         "ERROR-Tracker::~Tracker: nexi left unprocessed.\n");

  // 3. Check that all source signals processed
  if (source_signals_.get_num_signals() != 0) {
    fprintf(stdout, "WARNING: signals left unprocessed:\n");
    source_signals_.print();
  }
}

// Flip-Flop Status Tracking
void Tracker::set_inside_ff_block() { inside_ff_block_ = true; }

void Tracker::clear_inside_ff_block() { inside_ff_block_ = false; }

bool Tracker::check_if_inside_ff_block() const { return inside_ff_block_; }

bool Tracker::check_if_clk_signal(ivl_signal_t source_signal) const {
  if (std::string(ivl_signal_basename(source_signal)).find(clk_basename_) !=
      std::string::npos) {
    return true;
  } else {
    return false;
  }
}

// Source Signal Tracking
Signal* Tracker::get_source_signal(unsigned int index) const {
  return source_signals_.get_signal(index);
}

Signal* Tracker::pop_source_signal(std::string ws __attribute__((unused))) {
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                      "%spopping %d source signal(s) from stack\n", ws.c_str(),
                      1);)
  return source_signals_.pop_signal();
}

void Tracker::pop_source_signals(unsigned int num_signals,
                                 std::string ws __attribute__((unused))) {
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                      "%spopping %d source signal(s) from stack\n", ws.c_str(),
                      num_signals);)
  source_signals_.pop_signals(num_signals);
}

void Tracker::push_source_signal(Signal* source_signal, unsigned int id,
                                 std::string ws __attribute__((unused))) {
  assert(source_signal &&
         "ERROR: attempting to push NULL source signal to queue.\n");
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                      "%spushing source signal (name: %s, ID: %u) to stack\n",
                      ws.c_str(), source_signal->get_fullname().c_str(), id);)
  source_signals_.push_signal(source_signal, id);
}

// Slice Tracking
void Tracker::enable_slicing() { slicing_enabled_ = true; }

void Tracker::disable_slicing() { slicing_enabled_ = false; }

void Tracker::set_source_slice(Signal* signal, unsigned int msb,
                               unsigned int lsb, std::string ws) {
  if (slicing_enabled_) {
    signal->set_source_slice(msb, lsb, ws);
  }
}

void Tracker::set_sink_slice(Signal* signal, unsigned int msb, unsigned int lsb,
                             std::string ws) {
  if (slicing_enabled_) {
    signal->set_sink_slice(msb, lsb, ws);
  }
}

void Tracker::set_source_slice(Signal* signal, signal_slice_t slice,
                               std::string ws) {
  if (slicing_enabled_) {
    signal->set_source_slice(slice, ws);
  }
}

void Tracker::set_sink_slice(Signal* signal, signal_slice_t slice,
                             std::string ws) {
  if (slicing_enabled_) {
    signal->set_sink_slice(slice, ws);
  }
}

void Tracker::shift_source_slice(Signal* signal, int num_bits, std::string ws) {
  if (slicing_enabled_) {
    signal->shift_source_slice(num_bits, ws);
  }
}

void Tracker::shift_sink_slice(Signal* signal, int num_bits, std::string ws) {
  if (slicing_enabled_) {
    signal->shift_sink_slice(num_bits, ws);
  }
}

// Depth Tracking
unsigned int Tracker::pop_scope_depth() {
  assert(num_signals_at_depth_.size() > 0 &&
         "ERROR-Tracker::pop_scope_depth: cannot pop from empty queue.\n");
  unsigned int num_signals = num_signals_at_depth_.back();
  num_signals_at_depth_.pop_back();
  return num_signals;
}

void Tracker::push_scope_depth(unsigned int num_signals) {
  num_signals_at_depth_.push_back(num_signals);
}

// Config Loading
void Tracker::process_cmd_line_args(cmd_args_map_t* cmd_args) {
  clk_basename_ = cmd_args->at(CLK_BASENAME_FLAG);
}
