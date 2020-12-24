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
const char* Tracker::get_process_type_as_string(ivl_process_t process) {
  switch (ivl_process_type(process)) {
    case IVL_PR_ALWAYS:
      return "IVL_PR_ALWAYS";
    case IVL_PR_INITIAL:
      return "IVL_PR_INITIAL";
    case IVL_PR_FINAL:
      return "IVL_PR_FINAL";
    default:
      return "UNKOWN";
  }
}

// Main PROCESSING Function
int Tracker::process_process(ivl_process_t process) {
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "processing process (%s)\n",
                      get_process_type_as_string(process));)

  switch (ivl_process_type(process)) {
    case IVL_PR_ALWAYS:
      // Check if already in process block.
      // Nested process blocks are not supported.
      if (check_if_inside_ff_block()) {
        Error::not_supported("nested process blocks.");
      }

      // Check that the always block is not an analog process
      if (ivl_process_analog(process)) {
        Error::not_supported("analog IVL_PR_ALWAYS process statement.");
      }

      // Process statment
      process_statement(ivl_process_stmt(process), WS_TAB);

      // Check if inside ff flag is set.
      // If so, clear it.
      if (check_if_inside_ff_block()) {
        clear_inside_ff_block();
      }

      break;
    case IVL_PR_INITIAL:
      DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                          "WARNING: Ignoring INITIAL block...\n");)
      break;
    case IVL_PR_FINAL:
      DEBUG_PRINT(
          fprintf(DEBUG_PRINTS_FILE_PTR, "WARNING: Ignoring FINAL block...\n");)
      break;
    default:
      Error::not_supported("process statement type (UNKOWN).");
      break;
  }

  return 0;
}
