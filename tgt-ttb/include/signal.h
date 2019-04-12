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
    // IVL_LPM    = 4,
    // IVL_LOGIC  = 5,
} ivl_obj_type_t;

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
        string         get_fullname()   const;
        string         get_basename()   const;
        ivl_object_t   get_ivl_obj()    const;
        ivl_obj_type_t get_ivl_type()   const;
        ivl_signal_t   get_ivl_signal() const;
        unsigned       get_msb()        const;
        unsigned       get_lsb()        const;
        bool 		   is_signal()      const;
		bool 		   is_const()       const;

        // Dot Getters
        string get_dot_label() const;
        string get_dot_shape() const;

        // General Setters
        void set_is_ff();
        void set_is_input();

        // Other
        bool         is_ivl_generated() const;
        unsigned int process_as_partselect_expr(ivl_statement_t statement) const;

    private:
        ivl_object_t   ivl_object_;
        ivl_obj_type_t ivl_type_;
        unsigned int   id_;
        bool           is_ff_;
        bool           is_input_;

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
