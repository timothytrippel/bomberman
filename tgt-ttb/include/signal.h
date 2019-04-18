/*
File:        signal.h
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This is an Icarus Verilog backend target module that generates a signal
dependency graph for a given circuit design. The output format is a 
Graphviz .dot file.
*/

#ifndef __SIGNAL_HEADER__
#define __SIGNAL_HEADER__

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

// Dot Graph Shapes
#define SIGNAL_NODE_SHAPE       "ellipse"
#define LOCAL_SIGNAL_NODE_SHAPE "none"
#define FF_NODE_SHAPE           "square"
#define INPUT_NODE_SHAPE        "rectangle"
#define CONST_NODE_SHAPE        "none"

// Other Defines
#define BITSTRING_BASE 2

// ------------------------------------------------------------
// ------------- Underlying IVL Object of Signal --------------
// ------------------------------------------------------------

// Union for holding a signal IVL object.
// Signals (nodes in Dot graph) can either be an IVL signal or 
// an IVL constant. Constants can additionally come in two 
// forms: an net_const object or an expression object.
typedef union ivl_object_u {
    ivl_signal_t    ivl_signal;
    ivl_net_const_t ivl_const;
    ivl_expr_t      ivl_expr;

    // Constructors
    ivl_object_u():
        ivl_signal(NULL) {}

    ivl_object_u(ivl_signal_t sig):
        ivl_signal(sig) {}

    ivl_object_u(ivl_net_const_t net_const): 
        ivl_const(net_const) {}

    ivl_object_u(ivl_expr_t expr): 
        ivl_expr(expr) {}
} ivl_object_t;

// ------------------------------------------------------------
// --------- Type of Underlying IVL Object of Signal ----------
// ------------------------------------------------------------

typedef enum ivl_obj_type_e {
    IVL_NONE   = 0,
    IVL_SIGNAL = 1,
    IVL_CONST  = 2,
    IVL_EXPR   = 3
} ivl_obj_type_t;

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
// -------------------------- Signal --------------------------
// ------------------------------------------------------------

class Signal {
    public:
        // Constructors
        Signal();
        Signal(ivl_signal_t    signal);
        Signal(ivl_net_const_t constant);
        Signal(ivl_expr_t      expression);

        // Operators
        bool operator==(const Signal& sig) const;
        bool operator!=(const Signal& sig) const;

        // General Getters
        string         get_fullname()     const;
        string         get_basename()     const;
        ivl_object_t   get_ivl_obj()      const;
        ivl_obj_type_t get_ivl_type()     const;
        ivl_signal_t   get_ivl_signal()   const;
        unsigned int   get_msb()          const;
        unsigned int   get_lsb()          const;
        unsigned int   get_id()           const;
        unsigned int   get_array_base()   const;
        unsigned int   get_array_count()  const;
        bool 		   is_signal()        const;
		bool 		   is_const()         const;
        bool           is_arrayed()       const;
        bool           is_source_slice_modified() const;
        bool           is_sink_slice_modified()   const;
        signal_slice_t get_source_slice(Signal* signal) const;
        signal_slice_t get_sink_slice(Signal* signal)   const;

        // Dot Getters
        string get_dot_label() const;
        string get_dot_shape() const;

        // General Setters
        void set_is_ff();
        void set_is_input();
        void set_id(unsigned int value);
        void reset_slices();
        void set_source_slice(unsigned int msb, unsigned int lsb, string ws);
        void set_sink_slice(unsigned int msb, unsigned int lsb, string ws);
        void set_source_slice(signal_slice_t source_slice, string ws);
        void set_sink_slice(signal_slice_t sink_slice, string ws);
        void update_source_slice(unsigned int msb, unsigned int lsb, string ws);
        void update_sink_slice(unsigned int msb, unsigned int lsb, string ws);
        void update_source_slice(signal_slice_t source_slice, string ws);
        void update_sink_slice(signal_slice_t sink_slice, string ws);

        // Other
        bool         is_ivl_generated() const;
        unsigned int process_as_partselect_expr(ivl_statement_t statement) const;

    private:
        ivl_object_t   ivl_object_;
        ivl_obj_type_t ivl_type_;
        unsigned int   id_;
        bool           is_ff_;
        bool           is_input_;
        bool           source_slice_modified_;
        bool           sink_slice_modified_;
        unsigned int   source_msb_;
        unsigned int   source_lsb_;
        unsigned int   sink_msb_;
        unsigned int   sink_lsb_;

        // Unique ID for Constants
        static unsigned int const_id; 

        // Signal Getters
        string       get_signal_scopename() const;
        string       get_signal_basename()  const;
        string       get_signal_fullname()  const;
        unsigned int get_signal_msb()       const;
        unsigned int get_signal_lsb()       const;
        string       get_dot_signal_label() const;
        string       get_dot_const_label()  const;
        string       get_dot_expr_label()   const;

        // Constant Getters
        string       get_const_bitstring() const;
        string       get_const_fullname()  const;
        unsigned int get_const_msb()       const;

        // Expression Getters
        string       get_expr_bitstring() const;
        string       get_expr_fullname()  const;
        unsigned int get_expr_msb()       const;
};

// ------------------------------------------------------------
// -------------- IVL Signal to (TTB) Signal Map --------------
// ------------------------------------------------------------

typedef map<ivl_signal_t, Signal*> sig_map_t;

// ------------------------------------------------------------
// -------------------- (TTB) Signal Queue --------------------
// ------------------------------------------------------------

typedef vector<Signal*> signals_q_t;

#endif
