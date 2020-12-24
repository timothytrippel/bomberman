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
const char* Tracker::get_logic_type_as_string(ivl_net_logic_t logic) {
  switch (ivl_logic_type(logic)) {
    case IVL_LO_NONE:
      return "IVL_LO_NONE";
    case IVL_LO_AND:
      return "IVL_LO_AND";
    case IVL_LO_BUF:
      return "IVL_LO_BUF";
    case IVL_LO_BUFIF0:
      return "IVL_LO_BUFIF0";
    case IVL_LO_BUFIF1:
      return "IVL_LO_BUFIF1";
    case IVL_LO_BUFT:
      return "IVL_LO_BUFT";
    case IVL_LO_BUFZ:
      return "IVL_LO_BUFZ";
    case IVL_LO_CMOS:
      return "IVL_LO_CMOS";
    case IVL_LO_NAND:
      return "IVL_LO_NAND";
    case IVL_LO_NMOS:
      return "IVL_LO_NMOS";
    case IVL_LO_NOR:
      return "IVL_LO_NOR";
    case IVL_LO_NOT:
      return "IVL_LO_NOT";
    case IVL_LO_NOTIF0:
      return "IVL_LO_NOTIF0";
    case IVL_LO_NOTIF1:
      return "IVL_LO_NOTIF1";
    case IVL_LO_OR:
      return "IVL_LO_OR";
    case IVL_LO_PMOS:
      return "IVL_LO_PMOS";
    case IVL_LO_PULLDOWN:
      return "IVL_LO_PULLDOWN";
    case IVL_LO_PULLUP:
      return "IVL_LO_PULLUP";
    case IVL_LO_RCMOS:
      return "IVL_LO_RCMOS";
    case IVL_LO_RNMOS:
      return "IVL_LO_RNMOS";
    case IVL_LO_RPMOS:
      return "IVL_LO_RPMOS";
    case IVL_LO_XNOR:
      return "IVL_LO_XNOR";
    case IVL_LO_XOR:
      return "IVL_LO_XOR";
    case IVL_LO_UDP:
      return "IVL_LO_UDP";
    default:
      return "UNKNOWN";
  }
}

// Main LOGIC Progation
void Tracker::propagate_logic(ivl_net_logic_t logic, ivl_nexus_t sink_nexus,
                              Signal* sink_signal, std::string ws) {
  // LOGIC device pin nexus
  ivl_nexus_t pin_nexus = NULL;

  // Get number of pins on LOGIC device.
  // Each LOGIC device pin is a Nexus.
  unsigned int num_pins = ivl_logic_pins(logic);

  // Pin 0 is the output. If the (root) nexus, is not the
  // same as the output nexus, then we do not propagate,
  // because this sink_nexus is an input not an output.
  if (ivl_logic_pin(logic, LOGIC_OUTPUT_PIN_NEXUS_INDEX) == sink_nexus) {
    // Iterate over all input pins (nexuses) of LOGIC device.
    // Pin 0 is the output, so start with pin 1.
    for (unsigned int i = 1; i < num_pins; i++) {
      pin_nexus = ivl_logic_pin(logic, i);

      // Propagate the nexus
      propagate_nexus(pin_nexus, sink_signal, ws + WS_TAB);
    }
  }
}
