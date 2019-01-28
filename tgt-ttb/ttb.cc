// Standard Headers
#include <cassert>
#include <cstdio>

// TTB Headers
#include "ttb.h"
// #include "ttb_signal.h"

// *** "Main"/Entry Point *** of iverilog target
int target_design(ivl_design_t des) {
	printf("Hello World!\n");
	// ivl_scope_t* 	     roots = 0;     // root scopes of the design
	// unsigned 		     num_roots;     // number of root scopes of the design
	// Dot_File 		     df;  		    // output graph dot file
	// vector<ivl_signal_t> critical_sigs; // critical signals found in a design

	// // Variables to calculate runtime of this target module
	// double  duration;	
	// clock_t start = clock(); // Start timer

	// // Initialize graphviz dot file
	// df = Dot_File(ivl_design_flag(des, "-o"));
	// df.init_graph();

	// // Get root scopes of design
	// ivl_design_roots(des, &roots, &num_roots);
	
	// // Find all critical signals and dependencies in the design
	// find_critical_sigs(roots, num_roots, critical_sigs, CRITICAL_SIG_REGEX);

	// // Find signal dependencies of critical sigs
	// find_all_signal_dependencies(critical_sigs, df);

	// // Close graphviz dot file
	// df.save_graph();

	// // Stop timer
	// duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
	// printf("Execution Time: %f (s)\n\n", duration);

	return 0;
}
