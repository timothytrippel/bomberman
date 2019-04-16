/*
File:        connection.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __CONNECTION_HEADER__
#define __CONNECTION_HEADER__

// ------------------------------------------------------------
// ------------------------- Includes -------------------------
// ------------------------------------------------------------

// Standard Headers
#include <string>

// IVL API Header
#include <ivl_target.h>

// TTB Headers
#include "signal.h"

// Import STD Namespace
using namespace std;

// ------------------------------------------------------------
// ------------------------ Signal Slice ----------------------
// ------------------------------------------------------------
// Struct for holding MSB-LSB pair for tracking signal vector 
// slices at a given nexus
typedef struct signal_slice_s {
    unsigned int msb;
    unsigned int lsb;
} signal_slice_t;

// ------------------------------------------------------------
// ------------------------- Connection -----------------------
// ------------------------------------------------------------

class Connection {
    public:
        // Constructors
        Connection();

        Connection(Signal* source, 
                   Signal* sink);

        Connection(Signal*        source, 
                   Signal*        sink, 
                   signal_slice_t source_slice, 
                   signal_slice_t sink_slice);
        
        // Destructors
        ~Connection();

        // Operators
        bool operator==(const Connection& conn) const;
        bool operator!=(const Connection& conn) const;

        // General Getters
        Signal*      get_source()     const;
        Signal*      get_sink()       const;
        unsigned int get_source_id()  const;
        unsigned int get_sink_id()    const;
        unsigned int get_source_msb() const;
        unsigned int get_source_lsb() const;
        unsigned int get_sink_msb()   const;
        unsigned int get_sink_lsb()   const;

        // Dot Getters
        string get_dot_label() const;

        // General Setters
        void set_sink(Signal* new_sink);

    private:
        // Source Signal
        Signal*      source_;
        unsigned int source_msb_;
        unsigned int source_lsb_;
        unsigned int source_id_;  // for arrayed signals

        // Sink Signal
        Signal*      sink_;
        unsigned int sink_msb_;
        unsigned int sink_lsb_;
        unsigned int sink_id_;  // for arrayed signals
};

// ------------------------------------------------------------
// ----------------- (TTB) Connections Queue ------------------
// ------------------------------------------------------------

typedef vector<Connection*> conn_q_t;

// ------------------------------------------------------------
// -------- (TTB) Sink Signal to (TTB) Connection Map ---------
// ------------------------------------------------------------

typedef map<Signal*, conn_q_t*> conn_map_t;

#endif
