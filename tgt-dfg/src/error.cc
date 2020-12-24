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

#include "tgt-dfg/include/error.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/signal.h"

// Data Validation
void Error::check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes) {
  ivl_scope_type_t scope_type = IVL_SCT_MODULE;
  std::string scope_type_name = "UNKNOWN";
  bool scope_type_supported = false;

  for (unsigned int i = 0; i < num_scopes; i++) {
    // Get scope type
    scope_type = ivl_scope_type(scopes[i]);

    // Check scope is of type "module"
    assert(scope_type == IVL_SCT_MODULE &&
           "UNSUPPORTED: Verilog scope type.\n");

    // Get scope types
    switch (scope_type) {
      case IVL_SCT_MODULE:
        scope_type_name = "IVL_SCT_MODULE";
        scope_type_supported = true;
        break;
      case IVL_SCT_FUNCTION:
        scope_type_name = "IVL_SCT_FUNCTION";
        break;
      case IVL_SCT_TASK:
        scope_type_name = "IVL_SCT_TASK";
        // scope_type_supported = true;
        break;
      case IVL_SCT_BEGIN:
        scope_type_name = "IVL_SCT_BEGIN";
        // scope_type_supported = true;
        break;
      case IVL_SCT_FORK:
        scope_type_name = "IVL_SCT_FORK";
        break;
      case IVL_SCT_GENERATE:
        scope_type_name = "IVL_SCT_GENERATE";
        break;
      case IVL_SCT_PACKAGE:
        scope_type_name = "IVL_SCT_PACKAGE";
        break;
      case IVL_SCT_CLASS:
        scope_type_name = "IVL_SCT_CLASS";
        break;
      default:
        scope_type_name = "UNKNOWN";
        break;
    }

    // Check if scope type supported
    if (!scope_type_supported) {
      fprintf(stderr, "NOT-SUPPORTED: verilog scope type: %s",
              scope_type_name.c_str());
      fprintf(stderr, "File: %s Line: %d\n", ivl_scope_file(scopes[i]),
              ivl_scope_lineno(scopes[i]));
      exit(NOT_SUPPORTED_ERROR);
    }
  }
}

void Error::check_signal_exists_in_map(sig_map_t signals, ivl_signal_t signal) {
  if (signals.count(signal) > 0) {
    fprintf(stderr, "ERROR: signal (%s) already exists in hashmap.\n",
            signals[signal]->get_fullname().c_str());
    exit(DUPLICATE_SIGNALS_FOUND_ERROR);
  }
}

void Error::check_signal_not_arrayed(sig_map_t signals, ivl_signal_t signal) {
  // Check if signal valid
  if (signal) {
    // Check if signal is arrayed
    if (signals[signal]->is_arrayed()) {
      fprintf(stderr, "NOT-SUPPORTED: arrayed signal (%s -- %d) encountered.\n",
              signals[signal]->get_fullname().c_str(),
              ivl_signal_dimensions(signal));
      fprintf(stderr, "(base = %d; count = %d; addr_swapped = %d)\n",
              ivl_signal_array_base(signal), ivl_signal_array_count(signal),
              ivl_signal_array_addr_swapped(signal));
      exit(NOT_SUPPORTED_ERROR);
    } else {
      // Confirm that ARRAY_BASE is 0 (should be for non-arrayed signals)
      assert(ivl_signal_array_base(signal) == 0 &&
             "NOT-SUPPORTED: non-arrayed signal with non-zero ARRAY_BASE.");
      // Confirm that ARRAY_COUNT is a (should be for non-arrayed signals)
      assert(ivl_signal_array_count(signal) == 1 &&
             "NOT-SUPPORTED: non-arrayed signal with ARRAY_COUNT != 1.");
    }
  }
}

void Error::check_arrayed_signal(sig_map_t signals, ivl_signal_t signal) {
  // Check if signal valid
  if (signal) {
    // Check if signal is arrayed
    if (signals[signal]->is_arrayed()) {
      // Check if array base is negative
      if (ivl_signal_array_base(signal) < 0) {
        fprintf(stderr,
                "NOT-SUPPORTED: arrayed signal (%s) with negative base "
                "encountered.",
                signals[signal]->get_fullname().c_str());
        exit(NOT_SUPPORTED_ERROR);
      }
      // Check if number of dimensions is greater than 1
      if (ivl_signal_dimensions(signal) > 1) {
        fprintf(stderr,
                "NOT-SUPPORTED: arrayed signal (%s) with more than one dimen.",
                signals[signal]->get_fullname().c_str());
        exit(NOT_SUPPORTED_ERROR);
      }
    } else {
      // Confirm that ARRAY_BASE is 0 (should be for non-arrayed signals)
      assert(ivl_signal_array_base(signal) == 0 &&
             "NOT-SUPPORTED: non-arrayed signal with non-zero ARRAY_BASE.");
      // Confirm that ARRAY_COUNT is a (should be for non-arrayed signals)
      assert(ivl_signal_array_count(signal) == 1 &&
             "NOT-SUPPORTED: non-arrayed signal with ARRAY_COUNT != 1.");
    }
  }
}

void Error::check_signal_not_multidimensional(sig_map_t signals,
                                              ivl_signal_t signal) {
  // Check if signal is multidimensional
  if (ivl_signal_packed_dimensions(signal) > 1) {
    fprintf(stderr,
            "NOT-SUPPORTED: multidimensional signal (%s -- %d) encountered.\n",
            signals[signal]->get_fullname().c_str(),
            ivl_signal_packed_dimensions(signal));
    exit(NOT_SUPPORTED_ERROR);
  }
}

void Error::check_lvals_not_concatenated(unsigned int num_lvals,
                                         ivl_statement_t statement) {
  // Check for concatenated lvals
  if (num_lvals > 1) {
    fprintf(stderr,
            "WARNING: concatenated lvals not supported... skipping. \
            \n(File: %s -- Line: %d).\n",
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));
  }
}

void Error::check_lval_not_nested(ivl_lval_t lval, ivl_statement_t statement) {
  // Check if lval is nested
  if (ivl_lval_nest(lval)) {
    fprintf(stderr, "NOT-SUPPORTED: nested lvals (File: %s -- Line: %d).\n",
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));
    exit(PROCEDURAL_CONNECTIONS_ERROR);
  }
}

void Error::check_part_select_expr(ivl_obj_type_t obj_type,
                                   ivl_statement_t statement) {
  // Check part-select expression is only of type IVL_CONST_EXPR,
  // i.e. it is a constant expression
  if (obj_type != IVL_EXPR) {
    fprintf(stderr,
            "NOT-SUPPORTED: non-constant expression part-select (File: %s -- "
            "Line: %d).\n",
            ivl_stmt_file(statement), ivl_stmt_lineno(statement));
    exit(PROCEDURAL_CONNECTIONS_ERROR);
  }
}

// Error Reporting: Unkown Types
void Error::unknown_ivl_obj_type(ivl_obj_type_t obj_type) {
  fprintf(stderr, "ERROR: unkown ivl obj type (%d) encountered.\n", obj_type);
  exit(NOT_SUPPORTED_ERROR);
}

void Error::unknown_nexus_type() {
  fprintf(stderr, "ERROR: unkown nexus type for nexus.\n");
  exit(CONCURRENT_CONNECTIONS_ERROR);
}

void Error::unknown_signal_port_type(ivl_signal_port_t port_type) {
  fprintf(stderr, "ERROR: unkown signal port type (%d).\n", (int)port_type);
  exit(CONCURRENT_CONNECTIONS_ERROR);
}

void Error::unknown_signal_case(unsigned int case_type) {
  fprintf(stderr, "ERROR: unkown signal case (%u).\n", case_type);
  exit(CONCURRENT_CONNECTIONS_ERROR);
}

void Error::unknown_part_select_lpm_type(ivl_lpm_type_t lpm_type) {
  fprintf(stderr, "ERROR: unkown part select LPM type (%d).\n", (int)lpm_type);
  exit(CONCURRENT_CONNECTIONS_ERROR);
}

void Error::unknown_statement_type(ivl_statement_type_t statement_type) {
  fprintf(stderr, "ERROR: uknown statement type (%d).\n", (int)statement_type);
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::unknown_expression_type(ivl_expr_type_t expression_type) {
  fprintf(stderr, "ERROR: uknown expression type (%d).\n",
          (int)expression_type);
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

// Error Reporting: Other
void Error::not_supported(const char* message) {
  fprintf(stderr, "NOT-SUPPORTED: %s\n", message);
  exit(NOT_SUPPORTED_ERROR);
}

void Error::null_ivl_obj_type() {
  fprintf(stderr, "ERROR: node type IVL_NONE encountered.\n");
  exit(NOT_SUPPORTED_ERROR);
}

void Error::connecting_signal_not_in_graph(sig_map_t signals,
                                           ivl_signal_t source_signal) {
  fprintf(stderr, "ERROR: attempting to connect signal (%s) not in graph.\n",
          signals[source_signal]->get_fullname().c_str());
  exit(NOT_SUPPORTED_ERROR);
}

void Error::popping_source_signals_queue(unsigned int num_signals,
                                         unsigned int queue_size) {
  fprintf(stderr,
          "ERROR: attempting to pop %d signals from queue of size %d.\n",
          num_signals, queue_size);
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::processing_procedural_connections() {
  fprintf(stderr, "ERROR: processing behavioral logic connections.\n");
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::non_local_signal_connection() {
  fprintf(stderr,
          "ERROR: cannot to remove local signal between non-local signals.\n");
  exit(NOT_SUPPORTED_ERROR);
}

void Error::zero_event_nexus_ptrs(ivl_statement_t stmt) {
  fprintf(stderr,
          "NOT-SUPPORTED: 0 event nexus pointers. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::constant_event_nexus_ptr(ivl_statement_t stmt) {
  fprintf(stderr,
          "ERROR: constant event nexus pointer not supported. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::multiple_valid_event_nexus_ptrs(ivl_statement_t stmt) {
  fprintf(stderr,
          "NOT-SUPPORTED: >1 valid event nexus pointer candidate. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
  exit(PROCEDURAL_CONNECTIONS_ERROR);
}

void Error::infinite_loopback_assignment(Signal* sink_signal) {
  fprintf(
      stderr,
      "ERROR: continous loopback assignment sink/source slices are the same. \
        \n(Sink Signal: %s).\n",
      sink_signal->get_fullname().c_str());
  exit(CONCURRENT_CONNECTIONS_ERROR);
}

// Error Reporting: WARNINGS
void Error::constant_event_nexus_ptr_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: skipping constant event nexus pointer. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::unkown_event_source_signal_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: cannot determine event source signal... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::stask_statement_type_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: IVL_ST_STASK not supported... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::utask_statement_type_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: IVL_ST_UTASK not supported... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::while_statement_type_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: IVL_ST_WHILE not supported... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::repeat_statement_type_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: IVL_ST_REPEAT not supported... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::string_expression_type_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: IVL_EX_STRING not supported... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}

void Error::sfunc_expression_type_warning(ivl_statement_t stmt) {
  fprintf(stderr,
          "WARNING: IVL_EX_SFUNC not supported... skipping. \
        \n(File: %s -- Line: %d).\n",
          ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
}
