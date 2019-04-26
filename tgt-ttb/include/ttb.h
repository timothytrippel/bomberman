/*
File:        ttb.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __TTB_HEADER__
#define __TTB_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include <tracker.h>

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// -------------- CMD Line Arguments Processing ---------------
// ------------------------------------------------------------

cmd_args_map_t* process_cmd_line_args(ivl_design_t des);

// ------------------------------------------------------------
// ---------------- Launch IVL Design Process -----------------
// ------------------------------------------------------------

int find_procedural_connections(ivl_process_t process, void* t);
void launch_ivl_design_process(ivl_design_t design, Tracker* tracker); 

#endif
