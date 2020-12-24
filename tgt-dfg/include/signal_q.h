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

#ifndef TGT_DFG_INCLUDE_SIGNAL_Q_H_
#define TGT_DFG_INCLUDE_SIGNAL_Q_H_

#include <vector>

#include "iverilog/ivl_target.h"
#include "tgt-dfg/include/signal.h"

class SignalQ {
 public:
  // Constructors
  SignalQ();

  // Getters
  unsigned int get_num_signals() const;
  Signal* get_signal(unsigned int index) const;
  Signal* get_back_signal() const;
  Signal* get_back_signal(unsigned int index) const;
  Signal* pop_signal();
  void pop_signals(unsigned int num_signals);

  // Setters
  void push_signal(Signal* signal, unsigned int id);

  // Debug
  void print() const;

 private:
  std::vector<Signal*> signal_q_;
  std::vector<unsigned int> id_q_;
};

#endif  // TGT_DFG_INCLUDE_SIGNAL_Q_H_
