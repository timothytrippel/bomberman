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

#ifndef TGT_DFG_INCLUDE_SIGNAL_H_
#define TGT_DFG_INCLUDE_SIGNAL_H_

#include <map>
#include <string>
#include <vector>

#include "iverilog/ivl_target.h"

// Dot Graph Shapes
#define SIGNAL_NODE_SHAPE "ellipse"
#define LOCAL_SIGNAL_NODE_SHAPE "none"
#define FF_NODE_SHAPE "square"
#define INPUT_NODE_SHAPE "rectangle"
#define CONST_NODE_SHAPE "none"

#define BITSTRING_BASE 2

// Union for holding a signal IVL object.
// Signals (nodes in Dot graph) can either be an IVL signal or
// an IVL constant. Constants can additionally come in two
// forms: an net_const object or an expression object.
typedef union ivl_object_u {
  ivl_signal_t ivl_signal;
  ivl_net_const_t ivl_const;
  ivl_expr_t ivl_expr;

  // Constructors
  ivl_object_u() : ivl_signal(NULL) {}

  ivl_object_u(ivl_signal_t sig) : ivl_signal(sig) {}

  ivl_object_u(ivl_net_const_t net_const) : ivl_const(net_const) {}

  ivl_object_u(ivl_expr_t expr) : ivl_expr(expr) {}
} ivl_object_t;

typedef enum ivl_obj_type_e {
  IVL_NONE = 0,
  IVL_SIGNAL = 1,
  IVL_CONST = 2,
  IVL_EXPR = 3
} ivl_obj_type_t;

// Struct for holding MSB-LSB pair for tracking signal vector
// slices at a given nexus
typedef struct signal_slice_s {
  unsigned int msb;
  unsigned int lsb;
} signal_slice_t;

class Signal {
 public:
  // Constructors
  Signal();
  explicit Signal(ivl_signal_t signal);
  explicit Signal(ivl_net_const_t constant);
  explicit Signal(ivl_expr_t expression);

  // Operators
  bool operator==(const Signal& sig) const;
  bool operator!=(const Signal& sig) const;

  // Static Getters
  static std::string get_fullname(ivl_signal_t ivl_signal);
  static std::string get_const_bitstring(ivl_net_const_t ivl_const);

  // General Getters
  std::string get_fullname() const;
  std::string get_basename() const;
  ivl_object_t get_ivl_obj() const;
  ivl_obj_type_t get_ivl_type() const;
  ivl_signal_t get_ivl_signal() const;
  unsigned int get_msb() const;
  unsigned int get_lsb() const;
  unsigned int get_id() const;
  unsigned int get_array_base() const;
  unsigned int get_array_count() const;
  bool is_signal() const;
  bool is_const() const;
  bool is_const_expr() const;
  bool is_arrayed() const;
  bool is_source_slice_modified() const;
  bool is_sink_slice_modified() const;
  signal_slice_t get_source_slice(Signal* signal) const;
  signal_slice_t get_sink_slice(Signal* signal) const;

  // Dot Getters
  std::string get_dot_label() const;
  std::string get_dot_shape() const;

  // General Setters
  void set_is_ff();
  void set_is_input();
  void set_id(unsigned int value);
  void reset_source_slice();
  void reset_sink_slice();
  void reset_slices();
  void set_source_slice(unsigned int msb, unsigned int lsb, std::string ws);
  void set_sink_slice(unsigned int msb, unsigned int lsb, std::string ws);
  void set_source_slice(signal_slice_t source_slice, std::string ws);
  void set_sink_slice(signal_slice_t sink_slice, std::string ws);
  void shift_source_slice(int num_bits, std::string ws);
  void shift_sink_slice(int num_bits, std::string ws);

  // Other
  bool is_ivl_generated() const;
  unsigned int to_uint() const;

 private:
  ivl_object_t ivl_object_;
  ivl_obj_type_t ivl_type_;
  unsigned int id_;
  bool is_ff_;
  bool is_input_;
  bool source_slice_modified_;
  bool sink_slice_modified_;
  unsigned int source_msb_;
  unsigned int source_lsb_;
  unsigned int sink_msb_;
  unsigned int sink_lsb_;

  // Unique ID for Constants
  static unsigned int const_id;

  // Signal Getters
  std::string get_signal_scopename() const;
  std::string get_signal_basename() const;
  std::string get_signal_fullname() const;
  unsigned int get_signal_msb() const;
  unsigned int get_signal_lsb() const;
  std::string get_dot_signal_label() const;
  std::string get_dot_const_label() const;
  std::string get_dot_expr_label() const;

  // Constant Getters
  std::string get_const_bitstring() const;
  std::string get_const_fullname() const;
  unsigned int get_const_msb() const;

  // Expression Getters
  std::string get_expr_bitstring() const;
  std::string get_expr_fullname() const;
  unsigned int get_expr_msb() const;
};

typedef std::map<ivl_signal_t, Signal*> sig_map_t;
typedef std::map<unsigned int, Signal*> consts_map_t;
typedef std::map<ivl_signal_t, bool> sig_set_t;
typedef std::vector<Signal*> signals_q_t;

#endif  // TGT_DFG_INCLUDE_SIGNAL_H_
