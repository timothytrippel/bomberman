/*
File:        ttb_typedefs.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_TYPEDEFS_HEADER__
#define __TTB_TYPEDEFS_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <string>
#include <map>

// IVL API Header
#include <ivl_target.h>

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------- Defines --------------------------
// ------------------------------------------------------------

// Progress Messages
#define LAUNCH_MESSAGE                 "Entering TTB Target Module..."
#define LOADING_SIGS_TO_IGNORE_MESSAGE "Loading signals to ignore..."
#define SCOPE_EXPANSION_MESSAGE        "Identifying top-level modules..."
#define SIGNAL_ENUM_MESSAGE            "Enumerating signals..."
#define COMB_CONNECTION_ENUM_MESSAGE   "Enumerating combinational logic connections..."
#define BEHAVE_CONNECTION_ENUM_MESSAGE "Enumerating behavioral logic connections..."
#define LOCAL_CONNECTION_OPT_MESSAGE   "Processing local signal connections..."
#define STATS_MESSAGE                  "Analysis Complete."

// Define Indexes
#define LOGIC_OUTPUT_PIN_NEXUS_INDEX      0
#define LPM_PART_SELECT_INPUT_NEXUS_INDEX 0
#define LPM_PART_SELECT_BASE_NEXUS_INDEX  1
#define STMT_ASSIGN_LVAL_INDEX            0

// Other Defines
#define BITSTRING_BASE 2
#define LINE_SEPARATOR "------------------------------------------------------------"
#define WS_TAB         "  "

// Debugging Switches
#define DEBUG_PRINTS false

// ------------------------------------------------------------
// ------------------------ String Map ------------------------
// ------------------------------------------------------------

typedef map<string, bool> string_map_t;

// ------------------------------------------------------------
// --------------------- CMD-Line Args Map --------------------
// ------------------------------------------------------------

typedef map<string, string> cmd_args_map_t;

#endif
