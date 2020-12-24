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

#include <cassert>
#include <cstdio>

#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/error.h"
#include "tgt-dfg/include/tracker.h"

const char* Tracker::get_signal_port_type_as_string(ivl_signal_t signal) {
  switch (ivl_signal_port(signal)) {
    case IVL_SIP_NONE:
      return "IVL_SIP_NONE";
    case IVL_SIP_INPUT:
      return "IVL_SIP_INPUT";
    case IVL_SIP_OUTPUT:
      return "IVL_SIP_OUTPUT";
    case IVL_SIP_INOUT:
      return "IVL_SIP_INOUT";
    default:
      return "UNKOWN";
  }
}

bool Tracker::is_sig1_parent_of_sig2(ivl_signal_t signal_1,
                                     ivl_signal_t signal_2) {
  // Get signal scopes
  ivl_scope_t signal_1_signal_scope = ivl_signal_scope(signal_1);
  ivl_scope_t signal_2_signal_scope = ivl_signal_scope(signal_2);

  // Check if parent of sink scope is the source scope
  if (ivl_scope_parent(signal_2_signal_scope) == signal_1_signal_scope) {
    return true;
  } else {
    return false;
  }
}

bool Tracker::in_same_scope(ivl_signal_t signal_1, ivl_signal_t signal_2) {
  // Get signal scopes
  ivl_scope_t signal_1_scope = ivl_signal_scope(signal_1);
  ivl_scope_t signal_2_scope = ivl_signal_scope(signal_2);

  // Check if parent of sink scope is the source scope
  if (signal_1_scope == signal_2_scope) {
    return true;
  } else {
    return false;
  }
}

bool Tracker::process_signal_connect(ivl_signal_t ivl_source_signal,
                                     Signal* sink_signal, std::string ws) {
  // Get source signal
  Signal* source_signal = sg_->get_signal_from_ivl_signal(ivl_source_signal);

  // Add Connection
  return (sg_->add_connection(
      sink_signal, source_signal, sink_signal->get_sink_slice(sink_signal),
      sink_signal->get_source_slice(source_signal), ws + WS_TAB));
}

bool Tracker::process_signal_case(unsigned int case_num,
                                  ivl_signal_t ivl_source_signal,
                                  Signal* sink_signal, std::string ws) {
  // Get SOURCE signal port direction
  switch (case_num) {
    case 1:
      return process_signal_connect(ivl_source_signal, sink_signal, ws);
      break;
    case 2:
      break;
    case 3:
      return process_signal_connect(ivl_source_signal, sink_signal, ws);
      break;
    case 4:
      break;
    case 5:
      return process_signal_connect(ivl_source_signal, sink_signal, ws);
    case 6:
      // SOURCE MUST BE CHILD OF SINK
      if (is_sig1_parent_of_sig2(sink_signal->get_ivl_signal(),
                                 ivl_source_signal)) {
        return process_signal_connect(ivl_source_signal, sink_signal, ws);
      }
      break;
    case 7:
      break;
    case 8:
      // SOURCE MUST BE PARENT OF SINK
      if (is_sig1_parent_of_sig2(ivl_source_signal,
                                 sink_signal->get_ivl_signal())) {
        return process_signal_connect(ivl_source_signal, sink_signal, ws);
      }
      break;
    case 9:
      break;
    case 10:
      // SOURCE MUST BE PARENT OF SINK
      if (is_sig1_parent_of_sig2(ivl_source_signal,
                                 sink_signal->get_ivl_signal())) {
        return process_signal_connect(ivl_source_signal, sink_signal, ws);
      }
      break;
    case 11:
      break;
    case 12:
      break;
    case 13:
      return process_signal_connect(ivl_source_signal, sink_signal, ws);
      break;
    case 14:
      break;
    case 15:
      return process_signal_connect(ivl_source_signal, sink_signal, ws);
      break;
    case 16:
      break;
    case 17:
      return process_signal_connect(ivl_source_signal, sink_signal, ws);
      break;
    case 18:
      // SOURCE MUST BE CHILD OF SINK
      if (is_sig1_parent_of_sig2(sink_signal->get_ivl_signal(),
                                 ivl_source_signal)) {
        return process_signal_connect(ivl_source_signal, sink_signal, ws);
      }
      break;
    default:
      Error::unknown_signal_case(case_num);
      break;
  }

  return false;
}

bool Tracker::propagate_signal(ivl_signal_t ivl_source_signal,
                               Signal* sink_signal, std::string ws) {
  // Get IVL sink signal
  ivl_signal_t ivl_sink_signal = sink_signal->get_ivl_signal();

  // Get signal port directions
  ivl_signal_port_t source_port = ivl_signal_port(ivl_source_signal);
  ivl_signal_port_t sink_port = ivl_signal_port(ivl_sink_signal);

  // Process Signal Port Types
  if (source_port == IVL_SIP_NONE && sink_port == IVL_SIP_NONE) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 1 --> SOURCE-NONE -- SINK-NONE -- SAME MODULE
      return process_signal_case(1, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 2 --> SOURCE-NONE -- SINK-NONE -- DIFF MODULE
      return process_signal_case(2, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_INPUT && sink_port == IVL_SIP_NONE) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 3 --> SOURCE-INPUT -- SINK-NONE -- SAME MODULE
      return process_signal_case(3, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 4 --> SOURCE-INPUT -- SINK-NONE -- DIFF MODULE
      return process_signal_case(4, ivl_source_signal, sink_signal, ws);
    }
  } else if (source_port == IVL_SIP_OUTPUT && sink_port == IVL_SIP_NONE) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 5 --> SOURCE-OUTPUT -- SINK-NONE -- SAME MODULE
      return process_signal_case(5, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 6 --> SOURCE-OUTPUT -- SINK-NONE -- DIFF MODULE
      return process_signal_case(6, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_NONE && sink_port == IVL_SIP_INPUT) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 7 --> SOURCE-NONE -- SINK-INPUT -- SAME MODULE
      return process_signal_case(7, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 8 --> SOURCE-NONE -- SINK-INPUT -- DIFF MODULE
      return process_signal_case(8, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_INPUT && sink_port == IVL_SIP_INPUT) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 9 --> SOURCE-INPUT -- SINK-INPUT -- SAME MODULE
      return process_signal_case(9, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 10 --> SOURCE-INPUT -- SINK-INPUT -- DIFF MODULE
      return process_signal_case(10, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_OUTPUT && sink_port == IVL_SIP_INPUT) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 11 --> SOURCE-OUTPUT -- SINK-INPUT -- SAME MODULE
      return process_signal_case(11, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 12 --> SOURCE-OUTPUT -- SINK-INPUT -- DIFF MODULE
      return process_signal_case(12, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_NONE && sink_port == IVL_SIP_OUTPUT) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 13 --> SOURCE-NONE -- SINK-OUTPUT -- SAME MODULE
      return process_signal_case(13, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 14 --> SOURCE-NONE -- SINK-OUTPUT -- DIFF MODULE
      return process_signal_case(14, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_INPUT && sink_port == IVL_SIP_OUTPUT) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 15 --> SOURCE-INPUT -- SINK-OUTPUT -- SAME MODULE
      return process_signal_case(15, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 16 --> SOURCE-INPUT -- SINK-OUTPUT -- DIFF MODULE
      return process_signal_case(16, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_OUTPUT && sink_port == IVL_SIP_OUTPUT) {
    // Check if source and sink signals in same modules (scopes)
    if (in_same_scope(ivl_source_signal, ivl_sink_signal)) {
      // CASE 17 --> SOURCE-OUTPUT -- SINK-OUTPUT -- SAME MODULE
      return process_signal_case(17, ivl_source_signal, sink_signal, ws);
    } else {
      // CASE 18 --> SOURCE-OUTPUT -- SINK-OUTPUT -- DIFF MODULE
      return process_signal_case(18, ivl_source_signal, sink_signal, ws);
    }

  } else if (source_port == IVL_SIP_INOUT || sink_port == IVL_SIP_INOUT) {
    // INOUT
    DEBUG_PRINT(fprintf(stderr,
                        "WARNING: sink signal port type (IVL_SIP_INOUT) not "
                        "supported ... skipping.\n");)
    // Error::not_supported("sink signal port type (IVL_SIP_INOUT).");
  } else {
    // OTHER
    Error::unknown_signal_port_type(ivl_signal_port(ivl_source_signal));
  }

  return false;
}
