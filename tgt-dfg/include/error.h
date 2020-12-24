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

#ifndef TGT_DFG_INCLUDE_ERROR_H_
#define TGT_DFG_INCLUDE_ERROR_H_

#include <vector>

#include "iverilog/ivl_target.h"
#include "tgt-dfg/include/connection.h"
#include "tgt-dfg/include/signal.h"

typedef enum dfg_error_type_e {
  NO_ERROR = 0,
  NOT_SUPPORTED_ERROR = 1,
  FILE_ERROR = 2,
  DUPLICATE_SIGNALS_FOUND_ERROR = 3,
  CONCURRENT_CONNECTIONS_ERROR = 4,
  PROCEDURAL_CONNECTIONS_ERROR = 5,
  SLICE_TRACKING_ERROR = 6,
} dfg_error_type_t;

class Error {
 public:
  // Data Validation Functions
  static void check_scope_types(ivl_scope_t* scopes, unsigned int num_scopes);
  static void check_signal_exists_in_map(sig_map_t signals,
                                         ivl_signal_t signal);
  static void check_signal_not_arrayed(sig_map_t signals, ivl_signal_t signal);
  static void check_arrayed_signal(sig_map_t signals, ivl_signal_t signal);
  static void check_signal_not_multidimensional(sig_map_t signals,
                                                ivl_signal_t signal);
  static void check_lvals_not_concatenated(unsigned int num_lvals,
                                           ivl_statement_t statement);
  static void check_lval_not_nested(ivl_lval_t lval, ivl_statement_t statement);
  static void check_part_select_expr(ivl_obj_type_t obj_type,
                                     ivl_statement_t statement);

  // Error Reporting Functions
  // Unknown Types
  static void unknown_ivl_obj_type(ivl_obj_type_t obj_type);
  static void unknown_nexus_type();
  static void unknown_signal_port_type(ivl_signal_port_t port_type);
  static void unknown_signal_case(unsigned int case_type);
  static void unknown_part_select_lpm_type(ivl_lpm_type_t lpm_type);
  static void unknown_statement_type(ivl_statement_type_t statement_type);
  static void unknown_expression_type(ivl_expr_type_t expression_type);
  // Warnings
  static void constant_event_nexus_ptr_warning(ivl_statement_t stmt);
  static void stask_statement_type_warning(ivl_statement_t stmt);
  static void utask_statement_type_warning(ivl_statement_t stmt);
  static void while_statement_type_warning(ivl_statement_t stmt);
  static void repeat_statement_type_warning(ivl_statement_t stmt);
  static void string_expression_type_warning(ivl_statement_t stmt);
  static void sfunc_expression_type_warning(ivl_statement_t stmt);
  static void unkown_event_source_signal_warning(ivl_statement_t stmt);
  // Other
  static void not_supported(const char* message);
  static void null_ivl_obj_type();
  static void connecting_signal_not_in_graph(sig_map_t signals,
                                             ivl_signal_t source_signal);
  static void popping_source_signals_queue(unsigned int num_signals,
                                           unsigned int queue_size);
  static void processing_procedural_connections();
  static void non_local_signal_connection();
  static void multiple_valid_event_nexus_ptrs(ivl_statement_t stmt);
  static void zero_event_nexus_ptrs(ivl_statement_t stmt);
  static void constant_event_nexus_ptr(ivl_statement_t stmt);
  static void infinite_loopback_assignment(Signal* sink_signal);
};

#endif  // TGT_DFG_INCLUDE_ERROR_H_
