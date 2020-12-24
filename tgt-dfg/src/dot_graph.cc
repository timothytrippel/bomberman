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

#include "tgt-dfg/include/dot_graph.h"

#include <cassert>
#include <string>

#include "tgt-dfg/include/connection.h"
#include "tgt-dfg/include/dfg_typedefs.h"
#include "tgt-dfg/include/error.h"

// Constructors
DotGraph::DotGraph() : file_path_(NULL), file_ptr_(NULL) {}

DotGraph::DotGraph(std::string p) : file_ptr_(NULL) { set_file_path(p); }

// Destructors
DotGraph::~DotGraph() {
  DEBUG_DESTRUCTORS(fprintf(DESTRUCTOR_PRINTS_FILE_PTR,
                            "Executing DotGraph destructor...\n");)
  // Close file if its open and not STDOUT
  if (file_ptr_ && (file_ptr_ != stdout)) {
    close_file();
  }
}

// Getters
std::string DotGraph::get_file_path() const { return file_path_; }

// Setters
void DotGraph::set_file_path(std::string p) { file_path_ = p; }

// Graph Construction
void DotGraph::init_graph() {
  open_file();
  fprintf(file_ptr_, "digraph G {\n");
}

void DotGraph::add_node(Signal* signal,
                        std::string ws __attribute__((unused))) const {
  if (file_ptr_) {
    DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR, "%sADDING NODE (%s)\n",
                        ws.c_str(), signal->get_fullname().c_str());)
    // Print to dot file
    fprintf(file_ptr_, "\t\"%s\" [shape=%s, label=\"%s%s\"];\n",
            signal->get_fullname().c_str(), signal->get_dot_shape().c_str(),
            signal->get_fullname().c_str(), signal->get_dot_label().c_str());
  } else {
    fprintf(stderr, "ERROR: dot graph file (%s) not open.\n",
            !file_path_.empty() ? file_path_.c_str() : "stdout");
    exit(FILE_ERROR);
  }
}

void DotGraph::add_connection(Connection* conn,
                              std::string ws __attribute__((unused))) const {
  if (file_ptr_) {
    DEBUG_PRINT(fprintf(DEBUG_PRINTS_FILE_PTR,
                        "%sADDING CONNECTION: %s[%u:%u] --> %s[%u:%u]\n",
                        ws.c_str(), conn->get_source()->get_fullname().c_str(),
                        conn->get_source_msb(), conn->get_source_lsb(),
                        conn->get_sink()->get_fullname().c_str(),
                        conn->get_sink_msb(), conn->get_sink_lsb());)
    // Add connection to .dot file
    fprintf(file_ptr_, "\t\"%s\" -> \"%s\"[label=\"%s\"];\n",
            conn->get_source()->get_fullname().c_str(),
            conn->get_sink()->get_fullname().c_str(),
            conn->get_dot_label().c_str());
  } else {
    fprintf(stderr, "ERROR: dot graph file (%s) not open.\n",
            !file_path_.empty() ? file_path_.c_str() : "stdout");
    exit(FILE_ERROR);
  }
}

// File Operations
void DotGraph::save_graph() {
  if (file_ptr_) {
    fprintf(file_ptr_, "}\n");
    close_file();
  }
}

void DotGraph::open_file() {
  file_ptr_ = fopen(file_path_.c_str(), "w");
  if (!file_ptr_) {
    fprintf(stderr, "ERROR: Could not open file %s\n",
            !file_path_.empty() ? file_path_.c_str() : "stdout");
    exit(FILE_ERROR);
  }
}

void DotGraph::close_file() {
  fclose(file_ptr_);
  file_ptr_ = NULL;
}
