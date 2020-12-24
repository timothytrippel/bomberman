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

#ifndef TGT_DFG_INCLUDE_DFG_TYPEDEFS_H_
#define TGT_DFG_INCLUDE_DFG_TYPEDEFS_H_

#include <map>
#include <string>

#include "iverilog/ivl_target.h"

// Progress Messages
#define LAUNCH_MESSAGE "Entering DFG Target Module..."
#define CONFIGS_MESSAGE "Loading Configurations:"
#define INITIALIZE_SIG_GRAPH_MESSAGE "Intializing signals map..."
#define INITIALIZE_TRACKERS_MESSAGE "Intializing connection trackers..."
#define SCOPE_EXPANSION_MESSAGE "Identifying top-level modules..."
#define SIGNAL_ENUM_MESSAGE "Enumerating signals..."
#define COMB_CONNECTION_ENUM_MESSAGE \
  "Enumerating continuous logic connections..."
#define BEHAVE_CONNECTION_ENUM_MESSAGE \
  "Enumerating procedural logic connections..."
#define LOCAL_CONNECTION_OPT_MESSAGE "Processing local signal connections..."
#define SIGNAL_SAVING_MESSAGE "Saving signals to dot graph..."
#define DESTROY_MESSAGE "Destroying all objects..."
#define FINAL_STATS_MESSAGE "Analysis Complete."

// CMD-Line Argument Flags
#define OUTPUT_FILENAME_FLAG "-o"
#define CLK_BASENAME_FLAG "clk"
#define IGNORE_FILEPATH_FLAG "ignore_filepath"
#define IGNORE_CONSTANTS_FLAG "ignore_consts"

// CMD-Line Argument Defaults
#define MEM_SIG_SIZE_DEFAULT 128

// Define Indexes
#define LOGIC_OUTPUT_PIN_NEXUS_INDEX 0
#define LPM_PART_SELECT_INPUT_NEXUS_INDEX 0
#define LPM_PART_SELECT_BASE_NEXUS_INDEX 1
#define STMT_ASSIGN_LVAL_INDEX 0
#define SIGNAL_DIM_0_BIT_INDEX 0

// File Pointer Defines
#define REPORTER_PRINTS_FILE_PTR stdout
#define DEBUG_PRINTS_FILE_PTR stdout
#define DESTRUCTOR_PRINTS_FILE_PTR stdout

// Other Defines
#define LINE_SEPARATOR "-------------------------------------------------------"
#define WS_TAB "--"

// Enable/Disable Debug Printing
// #define DEBUG

// Debug Printing Macros
#ifdef DEBUG
#define DEBUG_PRINT(x) x
#define DEBUG_DESTRUCTORS(x) x
#else
#define DEBUG_PRINT(x)
#define DEBUG_DESTRUCTORS(x)
#endif

typedef std::map<std::string, bool> string_map_t;
typedef std::map<std::string, std::string> cmd_args_map_t;

#endif  // TGT_DFG_INCLUDE_DFG_TYPEDEFS_H_
