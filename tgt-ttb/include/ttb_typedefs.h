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
#define CONFIGS_MESSAGE                "Loading Configurations:"
#define INITIALIZE_SIG_GRAPH_MESSAGE   "Intializing signals map..."
#define INITIALIZE_TRACKERS_MESSAGE    "Intializing connection trackers..."
#define SCOPE_EXPANSION_MESSAGE        "Identifying top-level modules..."
#define SIGNAL_ENUM_MESSAGE            "Enumerating signals..."
#define COMB_CONNECTION_ENUM_MESSAGE   "Enumerating continuous logic connections..."
#define BEHAVE_CONNECTION_ENUM_MESSAGE "Enumerating procedural logic connections..."
#define LOCAL_CONNECTION_OPT_MESSAGE   "Processing local signal connections..."
#define SIGNAL_SAVING_MESSAGE          "Saving signals to dot graph..."
#define DESTROY_MESSAGE                "Destroying all objects..."
#define FINAL_STATS_MESSAGE            "Analysis Complete."

// CMD-Line Argument Flags
#define OUTPUT_FILENAME_FLAG  "-o"
#define CLK_BASENAME_FLAG     "clk"
#define IGNORE_FILEPATH_FLAG  "ignore_filepath"
#define IGNORE_CONSTANTS_FLAG "ignore_consts" 

// CMD-Line Argument Defaults
#define MEM_SIG_SIZE_DEFAULT 128

// Define Indexes
#define LOGIC_OUTPUT_PIN_NEXUS_INDEX      0
#define LPM_PART_SELECT_INPUT_NEXUS_INDEX 0
#define LPM_PART_SELECT_BASE_NEXUS_INDEX  1
#define STMT_ASSIGN_LVAL_INDEX            0
#define SIGNAL_DIM_0_BIT_INDEX            0

// File Pointer Defines
#define REPORTER_PRINTS_FILE_PTR   stdout
#define DEBUG_PRINTS_FILE_PTR      stdout
#define DESTRUCTOR_PRINTS_FILE_PTR stdout

// Other Defines
#define LINE_SEPARATOR "--------------------------------------------------------------------------------"
#define WS_TAB         "--"

// Enable/Disable Debug Printing
// #define DEBUG

// Debug Printing Macros
#ifdef DEBUG 
	#define DEBUG_PRINT(x)       x
	#define DEBUG_DESTRUCTORS(x) x
#else 
	#define DEBUG_PRINT(x)
	#define DEBUG_DESTRUCTORS(x)
#endif

// ------------------------------------------------------------
// ------------------------ String Map ------------------------
// ------------------------------------------------------------

typedef map<string, bool> string_map_t;

// ------------------------------------------------------------
// --------------------- CMD-Line Args Map --------------------
// ------------------------------------------------------------

typedef map<string, string> cmd_args_map_t; 

#endif
