 /*
File:        propagate_lpm.cc
Author:      Timothy Trippel
Affiliation: MIT Lincoln Laboratory
Description:

This function propagtes an output nexus connected to 
an LPM device. There are many different types of LPMs
that can be encountered (see ivl_target.h) and each
type must be handled individually. 
*/

// Standard Headers
#include <cassert>
#include <cstdio>

// TTB Headers
#include "ttb.h"
#include "error.h"

const char* SignalGraph::get_lpm_type_as_string(ivl_lpm_t lpm) {
	switch(ivl_lpm_type(lpm)) {
		case IVL_LPM_ABS:
			return "IVL_LPM_ABS";
		case IVL_LPM_ADD:
			return "IVL_LPM_ADD";
		case IVL_LPM_ARRAY:
			return "IVL_LPM_ARRAY";
		case IVL_LPM_CAST_INT:
			return "IVL_LPM_CAST_INT";
		case IVL_LPM_CAST_INT2:
			return "IVL_LPM_CAST_INT2";
		case IVL_LPM_CAST_REAL:
			return "IVL_LPM_CAST_REAL";
		case IVL_LPM_CONCAT:
			return "IVL_LPM_CONCAT";
		case IVL_LPM_CONCATZ:
			return "IVL_LPM_CONCATZ";
		case IVL_LPM_CMP_EEQ:
			return "IVL_LPM_CMP_EEQ";
		case IVL_LPM_CMP_EQX:
			return "IVL_LPM_CMP_EQX";
		case IVL_LPM_CMP_EQZ:
			return "IVL_LPM_CMP_EQZ";
		case IVL_LPM_CMP_EQ:
			return "IVL_LPM_CMP_EQ";
		case IVL_LPM_CMP_GE:
			return "IVL_LPM_CMP_GE";
		case IVL_LPM_CMP_GT:
			return "IVL_LPM_CMP_GT";
		case IVL_LPM_CMP_NE:
			return "IVL_LPM_CMP_NE";
		case IVL_LPM_CMP_NEE:
			return "IVL_LPM_CMP_NEE";
		case IVL_LPM_DIVIDE:
			return "IVL_LPM_DIVIDE";
		case IVL_LPM_FF:
			return "IVL_LPM_FF";
		case IVL_LPM_MOD:
			return "IVL_LPM_MOD";
		case IVL_LPM_MULT:
			return "IVL_LPM_MULT";
		case IVL_LPM_MUX:
			return "IVL_LPM_MUX";
		case IVL_LPM_PART_VP:
			return "IVL_LPM_PART_VP";
		case IVL_LPM_PART_PV:
			return "IVL_LPM_PART_PV";
		case IVL_LPM_POW:
			return "IVL_LPM_POW";
		case IVL_LPM_RE_AND:
			return "IVL_LPM_RE_AND";
		case IVL_LPM_RE_NAND:
			return "IVL_LPM_RE_NAND";
		case IVL_LPM_RE_NOR:
			return "IVL_LPM_RE_NOR";
		case IVL_LPM_RE_OR:
			return "IVL_LPM_RE_OR";
		case IVL_LPM_RE_XNOR:
			return "IVL_LPM_RE_XNOR";
		case IVL_LPM_RE_XOR:
			return "IVL_LPM_RE_XOR";
		case IVL_LPM_REPEAT:
			return "IVL_LPM_REPEAT";
		case IVL_LPM_SFUNC:
			return "IVL_LPM_SFUNC";
		case IVL_LPM_SHIFTL:
			return "IVL_LPM_SHIFTL";
		case IVL_LPM_SHIFTR:
			return "IVL_LPM_SHIFTR";
		case IVL_LPM_SIGN_EXT:
			return "IVL_LPM_SIGN_EXT";
		case IVL_LPM_SUB:
			return "IVL_LPM_SUB";
		case IVL_LPM_SUBSTITUTE:
			return "IVL_LPM_SUBSTITUTE";
		case IVL_LPM_UFUNC:
			return "IVL_LPM_UFUNC";
		default:
			return "UNKNOWN";
	}
}

void SignalGraph::process_lpm_part_select(ivl_lpm_t    lpm, \
                                          ivl_nexus_t  root_nexus, \
                                          ivl_signal_t root_signal) {

	// Device Nexuses
	ivl_nexus_t input_nexus = NULL;
	ivl_nexus_t base_nexus  = NULL;

	// Get input pin (0) nexus for part-select LPM device
	input_nexus = ivl_lpm_data(lpm, LPM_PART_SELECT_INPUT_NEXUS_INDEX);

	// Get base pin (1) nexus for part-select LPM device, which
	// may be NULL if not used is non-constant base is used.
	base_nexus = ivl_lpm_data(lpm, LPM_PART_SELECT_BASE_NEXUS_INDEX);
	if (base_nexus) {
		Error::not_supported_error("non-constant base for LPM part select device.");
	}

	// Get MSB and LSB of slice
	unsigned int msb        = ivl_lpm_base(lpm) + ivl_lpm_width(lpm) - 1;
	unsigned int lsb        = ivl_lpm_base(lpm);
	SliceInfo    slice_info = {msb, lsb, false, input_nexus};

	// Determine LPM type
	if (ivl_lpm_type(lpm) == IVL_LPM_PART_VP) {
		// part select: vector to part (VP: part select in rval)
		slice_info.slice_root = false;
	} else if (ivl_lpm_type(lpm) == IVL_LPM_PART_PV) {
		// part select: part to vector (PV: part select in lval)
		slice_info.slice_root = true;
	} else {
		Error::unknown_part_select_lpm_type_error(ivl_lpm_type(lpm));
	}

	signal_slices_.push_back(slice_info);
	propagate_nexus(input_nexus, root_signal);
}

void SignalGraph::propagate_lpm(ivl_lpm_t lpm, \
                                ivl_nexus_t root_nexus, \
                                ivl_signal_t root_signal) {

	// Add connections
	switch (ivl_lpm_type(lpm)) {
		
		case IVL_LPM_PART_VP:
		case IVL_LPM_PART_PV:
			// ivl_lpm_q() returns the output nexus. If the 
			// output nexus is NOT the same as the root nexus, 
			// then we do not propagate because this nexus is an 
			// to input to an LPM, not an output.
			if (ivl_lpm_q(lpm) == root_nexus) {
				process_lpm_part_select(lpm, root_nexus, root_signal);
			}
			break;
		// case IVL_LPM_CONCAT:
		// case IVL_LPM_CONCATZ: {
		// 	break;
		// }
		default:
			break;
	}
}
