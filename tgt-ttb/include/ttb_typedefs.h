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

// Standard Headers
#include <vector>
#include <map>

using namespace std;

// IVL API Header
#include  <ivl_target.h>

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

// Signal graph adjaceny list
typedef map<ivl_signal_t, vector<ivl_signal_t>> sig_map_t;

// ------------------------------------------------------------
// ------------------------ Node Slice ------------------------
// ------------------------------------------------------------
// Node that is sliced (either SINK or SOURCE)
typedef enum node_slice_type_e {
    SINK   = 0,
    SOURCE = 1
} node_slice_type_t;

// Struct for holding MSB-LSB pair for tracking 
// signal vector slices at a given nexus
typedef struct node_slice_s {
    unsigned int      msb;
    unsigned int      lsb;
    node_slice_type_t type;
} node_slice_t;

// ------------------------------------------------------------
// ------------------------ Graph Node ------------------------
// ------------------------------------------------------------
// Tagged union for holding a signal node object.
// Graph nodes can either be an IVL signal or 
// an IVL constant. Constants can additionally
// come in two forms: an net_const object or 
// an expression object.
typedef union node_object_u {
    ivl_signal_t    ivl_signal;
    ivl_net_const_t ivl_constant;
    ivl_expr_t      ivl_const_expr;
} node_object_t;

typedef enum node_type_e {
    IVL_NONE       = 0,
    IVL_SIGNAL     = 1,
    IVL_CONST      = 2,
    IVL_CONST_EXPR = 3,
} node_type_t;

typedef struct node_s {
    node_object_t object;
    node_type_t   type;
} node_t;

// Signal queue
typedef vector<node_t> node_q_t;

#endif
