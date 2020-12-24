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

#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/error.h"
#include "tgt-dfg/include/tracker.h"

// Helper Functions
const char* Tracker::get_const_type_as_string(ivl_net_const_t constant) {
  switch (ivl_const_type(constant)) {
    case IVL_VT_BOOL:
      return "IVL_VT_BOOL";
    case IVL_VT_LOGIC:
      return "IVL_VT_LOGIC";
    default:
      return "UNKOWN";
  }
}

// Main CONSTANT Progation
void Tracker::propagate_constant(ivl_net_const_t constant, Signal* sink_signal,
                                 std::string ws) {
  // Source signal
  Signal* source_signal = new Signal(constant);

  switch (ivl_const_type(constant)) {
    case IVL_VT_BOOL:
    case IVL_VT_LOGIC:

      // Add Connection
      sg_->add_connection(
          sink_signal, source_signal, sink_signal->get_sink_slice(sink_signal),
          sink_signal->get_source_slice(source_signal), ws + WS_TAB);

      break;
    default:
      Error::not_supported("CONSTANT device type (UNKNOWN)");
      break;
  }
}
