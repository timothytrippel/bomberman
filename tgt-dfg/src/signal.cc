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

#include "tgt-dfg/include/signal.h"

#include <algorithm>
#include <cassert>
#include <sstream>

#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/error.h"

// Constructors
Signal::Signal()
    : ivl_object_(),
      ivl_type_(IVL_NONE),
      id_(0),
      is_ff_(false),
      is_input_(false),
      source_slice_modified_(false),
      sink_slice_modified_(false),
      source_msb_(0),
      source_lsb_(0),
      sink_msb_(0),
      sink_lsb_(0) {}

// Signal
Signal::Signal(ivl_signal_t signal)
    : ivl_object_(signal),
      ivl_type_(IVL_SIGNAL),
      id_(0),
      is_ff_(false),
      is_input_(false),
      source_slice_modified_(false),
      sink_slice_modified_(false),
      source_msb_(0),
      source_lsb_(0),
      sink_msb_(0),
      sink_lsb_(0) {
  // Check if the signal is an input
  if (ivl_signal_port(signal) == IVL_SIP_INPUT) {
    set_is_input();
  }
}

// Net Constant
Signal::Signal(ivl_net_const_t constant)
    : ivl_object_(constant),
      ivl_type_(IVL_CONST),
      id_(const_id),
      is_ff_(false),
      is_input_(false),
      source_slice_modified_(false),
      sink_slice_modified_(false),
      source_msb_(0),
      source_lsb_(0),
      sink_msb_(0),
      sink_lsb_(0) {
  // Increment Constant ID counter
  const_id++;
}

// Constant Expression
Signal::Signal(ivl_expr_t expression)
    : ivl_object_(expression),
      ivl_type_(IVL_EXPR),
      id_(const_id),
      is_ff_(false),
      is_input_(false),
      source_slice_modified_(false),
      sink_slice_modified_(false),
      source_msb_(0),
      source_lsb_(0),
      sink_msb_(0),
      sink_lsb_(0) {
  // Increment Constant ID counter
  const_id++;
}

// Operators
bool Signal::operator==(const Signal& sig) const {
  if (this->ivl_type_ == sig.get_ivl_type()) {
    switch (this->ivl_type_) {
      case IVL_SIGNAL:
        return (this->ivl_object_.ivl_signal == sig.get_ivl_obj().ivl_signal);
      case IVL_CONST:
        return (this->ivl_object_.ivl_const == sig.get_ivl_obj().ivl_const);
      case IVL_EXPR:
        return (this->ivl_object_.ivl_expr == sig.get_ivl_obj().ivl_expr);
      default:
        return false;
    }
  } else {
    return false;
  }
}

bool Signal::operator!=(const Signal& sig) const { return !(*this == sig); }

// Static Getters
std::string Signal::get_fullname(ivl_signal_t ivl_signal) {
  std::string scopename =
      std::string(ivl_scope_name(ivl_signal_scope(ivl_signal)));
  std::string basename = std::string(ivl_signal_basename(ivl_signal));
  std::string fullname = scopename + std::string(".") + basename;

  return fullname;
}

std::string Signal::get_const_bitstring(ivl_net_const_t ivl_const) {
  // Get bitstring
  std::string bitstring = std::string(ivl_const_bits(ivl_const),
                                      (size_t)ivl_const_width(ivl_const));

  // Reverse bitstring to MSB->LSB order
  reverse(bitstring.begin(), bitstring.end());

  return bitstring;
}

// Getters
ivl_object_t Signal::get_ivl_obj() const {
  switch (ivl_type_) {
    case IVL_SIGNAL:
      return ivl_object_.ivl_signal;
    case IVL_CONST:
      return ivl_object_.ivl_const;
    case IVL_EXPR:
      return ivl_object_.ivl_expr;
    default:
      return ivl_object_t();
  }
}

ivl_obj_type_t Signal::get_ivl_type() const { return ivl_type_; }

ivl_signal_t Signal::get_ivl_signal() const {
  if (ivl_type_ == IVL_SIGNAL) {
    return ivl_object_.ivl_signal;
  } else {
    return NULL;
  }
}

std::string Signal::get_fullname() const {
  switch (ivl_type_) {
    case IVL_SIGNAL:
      return get_signal_fullname();
    case IVL_CONST:
      return get_const_fullname();
    case IVL_EXPR:
      return get_expr_fullname();
    default:
      return "NONE";
  }
}

std::string Signal::get_basename() const {
  switch (ivl_type_) {
    case IVL_SIGNAL:
      return get_signal_basename();
    case IVL_CONST:
      return get_const_bitstring();
    case IVL_EXPR:
      return get_expr_bitstring();
    default:
      return "NONE";
  }
}

unsigned int Signal::get_msb() const {
  switch (ivl_type_) {
    case IVL_SIGNAL:
      return get_signal_msb();
    case IVL_CONST:
      return get_const_msb();
    case IVL_EXPR:
      return get_expr_msb();
    default:
      return 0;
  }
}

unsigned int Signal::get_lsb() const {
  switch (ivl_type_) {
    case IVL_SIGNAL:
      return get_signal_lsb();
    case IVL_CONST:
    case IVL_EXPR:
    default:
      return 0;
  }
}

unsigned int Signal::get_id() const { return id_; }

unsigned int Signal::get_array_base() const {
  if (ivl_type_ == IVL_SIGNAL) {
    return ivl_signal_array_base(ivl_object_.ivl_signal);
  } else {
    return 0;
  }
}

unsigned int Signal::get_array_count() const {
  if (ivl_type_ == IVL_SIGNAL) {
    return ivl_signal_array_count(ivl_object_.ivl_signal);
  } else {
    return 1;
  }
}

bool Signal::is_signal() const {
  if (ivl_type_ == IVL_SIGNAL) {
    return true;
  } else {
    return false;
  }
}

bool Signal::is_const() const {
  if (ivl_type_ == IVL_CONST || ivl_type_ == IVL_EXPR) {
    return true;
  } else {
    return false;
  }
}

bool Signal::is_const_expr() const {
  if (ivl_type_ == IVL_EXPR) {
    return true;
  } else {
    return false;
  }
}

bool Signal::is_arrayed() const {
  if (ivl_type_ == IVL_SIGNAL) {
    if (ivl_signal_dimensions(ivl_object_.ivl_signal) > 0) {
      return true;
    }
  }

  return false;
}

bool Signal::is_source_slice_modified() const { return source_slice_modified_; }

bool Signal::is_sink_slice_modified() const { return sink_slice_modified_; }

signal_slice_t Signal::get_source_slice(Signal* signal) const {
  signal_slice_t source_slice = {0, 0};

  // Check that signal is not NULL
  assert(signal &&
         "ERROR-Signal::get_source_slice: cannot get slice of NULL signal.\n");

  // Check if tracking slice info
  if (source_slice_modified_) {
    // Update source slice
    source_slice.msb = source_msb_;
    source_slice.lsb = source_lsb_;

  } else {
    // Check if this Signal is the SOURCE/SINK
    if (this == signal) {
      // This is the SOURCE signal
      source_slice.msb = this->get_msb();
      source_slice.lsb = this->get_lsb();

    } else {
      // This is the SINK signal
      source_slice.msb = signal->get_msb();
      source_slice.lsb = signal->get_lsb();
    }
  }

  return source_slice;
}

signal_slice_t Signal::get_sink_slice(Signal* signal) const {
  signal_slice_t sink_slice = {0, 0};

  // Check that signal is not NULL
  assert(signal &&
         "ERROR-Signal::get_sink_slice: cannot get slice of NULL signal.\n");

  // Check if tracking slice info
  if (sink_slice_modified_) {
    // Update sink slice
    sink_slice.msb = sink_msb_;
    sink_slice.lsb = sink_lsb_;

  } else {
    // Check if this Signal is the SOURCE/SINK
    if (this == signal) {
      // This is the SINK signal
      sink_slice.msb = this->get_msb();
      sink_slice.lsb = this->get_lsb();

    } else {
      // This is the SOURCE signal
      sink_slice.msb = signal->get_msb();
      sink_slice.lsb = signal->get_lsb();
    }
  }

  return sink_slice;
}

// Signal
std::string Signal::get_signal_scopename() const {
  return ivl_scope_name(ivl_signal_scope(ivl_object_.ivl_signal));
}

std::string Signal::get_signal_basename() const {
  return ivl_signal_basename(ivl_object_.ivl_signal);
}

std::string Signal::get_signal_fullname() const {
  if (this->is_arrayed()) {
    return (get_signal_scopename() + std::string(".") + get_signal_basename() +
            std::string("$") + std::to_string((uint64_t)id_)) +
           std::string("$");
  } else {
    return (get_signal_scopename() + std::string(".") + get_signal_basename());
  }
}

unsigned int Signal::get_signal_msb() const {
  if (ivl_signal_packed_dimensions(ivl_object_.ivl_signal) > 0) {
    // Check MSB is not negative
    assert((ivl_signal_packed_msb(ivl_object_.ivl_signal,
                                  SIGNAL_DIM_0_BIT_INDEX) >= 0) &&
           "NOT-SUPPORTED: negative MSB index.\n");

    return ivl_signal_packed_msb(ivl_object_.ivl_signal,
                                 SIGNAL_DIM_0_BIT_INDEX);
  } else {
    return 0;
  }
}

unsigned int Signal::get_signal_lsb() const {
  if (ivl_signal_packed_dimensions(ivl_object_.ivl_signal) > 0) {
    // Check LSB is not negative
    assert((ivl_signal_packed_lsb(ivl_object_.ivl_signal,
                                  SIGNAL_DIM_0_BIT_INDEX) >= 0) &&
           "NOT-SUPPORTED: negative LSB index.\n");

    return ivl_signal_packed_lsb(ivl_object_.ivl_signal,
                                 SIGNAL_DIM_0_BIT_INDEX);
  } else {
    return 0;
  }
}

// Constant
std::string Signal::get_const_bitstring() const {
  // Get bitstring
  std::string bitstring =
      std::string(ivl_const_bits(ivl_object_.ivl_const),
                  (size_t)ivl_const_width(ivl_object_.ivl_const));

  // Reverse bitstring to MSB->LSB order
  reverse(bitstring.begin(), bitstring.end());

  return bitstring;
}

std::string Signal::get_const_fullname() const {
  // Get (unique) constant prefix
  // string scopename = ivl_scope_name(ivl_const_scope(ivl_object_.ivl_const));
  std::string const_prefix =
      std::string("const.") + std::to_string((uint64_t)id_) + std::string(".");

  return (const_prefix + get_const_bitstring());
}

unsigned int Signal::get_const_msb() const {
  // Check MSB is not negative
  assert((ivl_const_width(ivl_object_.ivl_const) >= 1) &&
         "NOT-SUPPORTED: negative MSB index.\n");

  return ivl_const_width(ivl_object_.ivl_const) - 1;
}

// Expression
std::string Signal::get_expr_bitstring() const {
  // Get bitstring
  std::string bitstring =
      std::string(ivl_expr_bits(ivl_object_.ivl_expr),
                  (size_t)ivl_expr_width(ivl_object_.ivl_expr));

  // Reverse bitstring to MSB->LSB order
  reverse(bitstring.begin(), bitstring.end());

  return bitstring;
}

std::string Signal::get_expr_fullname() const {
  // Get (unique) constant prefix
  std::string const_prefix =
      std::string("const.") + std::to_string((uint64_t)id_) + std::string(".");

  return (const_prefix + get_expr_bitstring());
}

unsigned int Signal::get_expr_msb() const {
  return ivl_expr_width(ivl_object_.ivl_expr) - 1;
}

// Dot Graph Getters
std::string Signal::get_dot_label() const {
  switch (ivl_type_) {
    case IVL_SIGNAL:
      return get_dot_signal_label();
    case IVL_CONST:
      return get_dot_const_label();
    case IVL_EXPR:
      return get_dot_expr_label();
    default:
      return "?";
  }
}

std::string Signal::get_dot_shape() const {
  switch (ivl_type_) {
    case IVL_CONST:
    case IVL_EXPR:
      return CONST_NODE_SHAPE;
    case IVL_SIGNAL:
    default:
      if (is_ff_) {
        return FF_NODE_SHAPE;
      } else if (is_input_) {
        return INPUT_NODE_SHAPE;
      } else {
        return SIGNAL_NODE_SHAPE;
      }
  }
}

// Signal
std::string Signal::get_dot_signal_label() const {
  std::stringstream ss;
  ss << "[";
  ss << get_signal_msb();
  ss << ":";
  ss << get_signal_lsb();
  ss << "]";
  return ss.str();
}

// Constant
std::string Signal::get_dot_const_label() const {
  std::stringstream ss;
  ss << "[";
  ss << get_const_msb();
  ss << ":0]";
  return ss.str();
}

// Expression
std::string Signal::get_dot_expr_label() const {
  std::stringstream ss;
  ss << "[";
  ss << get_expr_msb();
  ss << ":0]";
  return ss.str();
}

// Setters
void Signal::set_is_ff() {
  assert(this->is_signal() && "ERROR: constants cannot be flagged as a FF.\n");
  assert((ivl_signal_type(ivl_object_.ivl_signal) == IVL_SIT_REG) &&
         "ERROR: non-reg type signals cannot be flagged as a FF.\n");
  is_ff_ = true;
}

void Signal::set_is_input() {
  assert(this->is_signal() &&
         "ERROR: constants cannot be flagged as an INPUT.\n");
  is_input_ = true;
}

void Signal::set_id(unsigned int value) {
  assert(this->is_signal() &&
         "ERROR: ID of constants cannot be manipulated.\n");
  id_ = value;
}

void Signal::reset_source_slice() {
  source_msb_ = 0;
  source_lsb_ = 0;
  source_slice_modified_ = false;
}

void Signal::reset_sink_slice() {
  sink_msb_ = 0;
  sink_lsb_ = 0;
  sink_slice_modified_ = false;
}

void Signal::reset_slices() {
  reset_source_slice();
  reset_sink_slice();
}

void Signal::set_source_slice(unsigned int msb, unsigned int lsb,
                              std::string ws __attribute__((unused))) {
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "%sSetting SOURCE slice [%u:%u]\n",
                      ws.c_str(), msb, lsb);)
  source_slice_modified_ = true;
  source_msb_ = msb;
  source_lsb_ = lsb;
}

void Signal::set_sink_slice(unsigned int msb, unsigned int lsb,
                            std::string ws __attribute__((unused))) {
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "%sSetting SINK slice [%u:%u]\n",
                      ws.c_str(), msb, lsb);)
  sink_slice_modified_ = true;
  sink_msb_ = msb;
  sink_lsb_ = lsb;
}

void Signal::set_source_slice(signal_slice_t source_slice,
                              std::string ws __attribute__((unused))) {
  this->set_source_slice(source_slice.msb, source_slice.lsb, ws);
}

void Signal::set_sink_slice(signal_slice_t sink_slice,
                            std::string ws __attribute__((unused))) {
  this->set_sink_slice(sink_slice.msb, sink_slice.lsb, ws);
}

void Signal::shift_source_slice(int num_bits,
                                std::string ws __attribute__((unused))) {
  // Negative is left shift, Positive is right shift
  // Check that resulting MSB and LSB are not negative
  assert(
      ((int)source_msb_ + num_bits) >= 0 &&
      ((int)source_lsb_ + num_bits) >= 0 &&
      "ERROR-Signal::shift_source_slice: shift will cause negative indices.\n");
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                      "%sShifting SOURCE slice [%u:%u] by %d to [%u:%u]\n",
                      ws.c_str(), source_msb_, source_lsb_, num_bits,
                      source_msb_ + num_bits, source_lsb_ + num_bits);)

  // Shift MSB/LSB
  source_lsb_ += num_bits;
  source_msb_ += num_bits;

  // Update modified flag
  source_slice_modified_ = true;
}

void Signal::shift_sink_slice(int num_bits,
                              std::string ws __attribute__((unused))) {
  // Negative is left shift, Positive is right shift
  assert(
      ((int)sink_msb_ + num_bits) >= 0 && ((int)sink_lsb_ + num_bits) >= 0 &&
      "ERROR-Signal::shift_sink_slice: shift will cause negative indices.\n");
  DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                      "%sShifting SINK slice [%u:%u] by %d to [%u:%u]\n",
                      ws.c_str(), sink_msb_, sink_lsb_, num_bits,
                      sink_msb_ + num_bits, sink_lsb_ + num_bits);)

  // Shift MSB/LSB
  sink_lsb_ += num_bits;
  sink_msb_ += num_bits;

  // Update modified flag
  sink_slice_modified_ = true;
}

// Other
bool Signal::is_ivl_generated() const {
  if (ivl_type_ == IVL_SIGNAL) {
    return ivl_signal_local(ivl_object_.ivl_signal);
  } else {
    return false;
  }
}

unsigned int Signal::to_uint() const {
  assert(this->is_const_expr() &&
         "ERROR-Signal::process_as_index_expr: cannot convert signals to "
         "numbers.\n");
  std::string bit_string = this->get_expr_bitstring();
  return stoul(bit_string, NULL, BITSTRING_BASE);
}
