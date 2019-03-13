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

// IVL API Header
#include <ivl_target.h>

// ------------------------------------------------------------
// ------------------------- Defines --------------------------
// ------------------------------------------------------------

// Progress Messages
#define LAUNCH_MESSAGE                 "Entering TTB Target Module..."
#define SCOPE_EXPANSION_MESSAGE        "Identifying top-level modules..."
#define SIGNAL_ENUM_MESSAGE            "Enumerating signals..."
#define COMB_CONNECTION_ENUM_MESSAGE   "Enumerating combinational logic connections..."
#define BEHAVE_CONNECTION_ENUM_MESSAGE "Enumerating behavioral logic connections..."

// Define Indexes
#define LOGIC_OUTPUT_PIN_NEXUS_INDEX      0
#define LPM_PART_SELECT_INPUT_NEXUS_INDEX 0
#define LPM_PART_SELECT_BASE_NEXUS_INDEX  1

// Other Defines
#define BITSTRING_BASE 2
#define LINE_SEPARATOR "-----------------------------"
#define WS_TAB         "  "

// Debugging Switches
#define DEBUG_PRINTS false

#endif
