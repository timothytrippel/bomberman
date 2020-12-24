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

#include "tgt-dfg/include/signal_q.h"

#include <cassert>

#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/error.h"

// Constructors
SignalQ::SignalQ() : signal_q_(), id_q_() {}

// Getters
unsigned int SignalQ::get_num_signals() const { return signal_q_.size(); }

Signal* SignalQ::get_signal(unsigned int index) const {
  // Check that queues are not empty, and are the same size
  assert(signal_q_.size() > 0 &&
         "ERROR-SignalQ::get_signal: cannot pop from empty signal queue.\n");
  assert(id_q_.size() > 0 &&
         "ERROR-SignalQ::get_signal: cannot pop from empty ID queue.\n");
  assert(signal_q_.size() == id_q_.size() &&
         "ERROR-SignalQ::get_signal: signal and ID queues should be the same "
         "size.\n");

  // Check that index is within the size bounds
  assert(index < signal_q_.size() &&
         "ERROR-SignalQ::get_signal: index outsize queue bounds.\n");

  // Signal/ID to return
  Signal* signal = signal_q_[index];
  unsigned int id = id_q_[index];

  // Set signal ID (arrayed source signals)
  if (signal->is_signal()) {
    signal->set_id(id);
  }

  return signal;
}

Signal* SignalQ::get_back_signal() const {
  return this->get_signal(signal_q_.size() - 1);
}

Signal* SignalQ::get_back_signal(unsigned int index) const {
  return this->get_signal(signal_q_.size() - index - 1);
}

Signal* SignalQ::pop_signal() {
  // Check that queues are not empty, and are the same size
  assert(signal_q_.size() > 0 &&
         "ERROR-SignalQ::pop_signal: cannot pop from empty signal queue.\n");
  assert(id_q_.size() > 0 &&
         "ERROR-SignalQ::pop_signal: cannot pop from empty ID queue.\n");
  assert(signal_q_.size() == id_q_.size() &&
         "ERROR-SignalQ::pop_signal: signal and ID queues should be the same "
         "size.\n");

  // Signal/ID to return
  Signal* signal = signal_q_.back();
  unsigned int id = id_q_.back();

  // Pop last items in each queue
  signal_q_.pop_back();
  id_q_.pop_back();

  // Set signal ID (arrayed source signals)
  if (signal->is_signal()) {
    signal->set_id(id);
  }

  return signal;
}

void SignalQ::pop_signals(unsigned int num_signals) {
  // Check that the queue is large enough
  assert(signal_q_.size() >= num_signals &&
         "ERROR-SignalQ::pop_slices: signal queue not large enough to pop "
         "<num_signals>.\n");

  // Pop signals/IDs
  for (unsigned int i = 0; i < num_signals; i++) {
    // Reset signal slices
    signal_q_.back()->reset_slices();

    // Remove items from queues
    signal_q_.pop_back();
    id_q_.pop_back();
  }
}

// Setters
void SignalQ::push_signal(Signal* signal, unsigned int id) {
  // Push slice to the queue
  signal_q_.push_back(signal);
  id_q_.push_back(id);
}

// Debug
void SignalQ::print() const {
  for (unsigned int i = 0; i < signal_q_.size(); i++) {
    fprintf(stdout, "%s%s\n", WS_TAB, get_signal(i)->get_fullname().c_str());
  }
}
