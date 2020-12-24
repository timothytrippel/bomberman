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

#ifndef TGT_DFG_INCLUDE_DOT_GRAPH_H_
#define TGT_DFG_INCLUDE_DOT_GRAPH_H_

#include <cstdio>
#include <string>

#include "iverilog/ivl_target.h"
#include "tgt-dfg/include/connection.h"
#include "tgt-dfg/include/signal.h"

class DotGraph {
 public:
  // Constructors
  DotGraph();
  explicit DotGraph(std::string p);

  // Destructors
  ~DotGraph();

  // Getters
  std::string get_file_path() const;

  // Setters
  void set_file_path(std::string p);

  // Graph Construction
  void init_graph();
  void add_node(Signal* signal, std::string ws) const;
  void add_connection(Connection* conn, std::string ws) const;
  void save_graph();

 private:
  std::string file_path_;
  FILE* file_ptr_;

  // File Operations
  void open_file();
  void close_file();
};

#endif  // TGT_DFG_INCLUDE_DOT_GRAPH_H_
