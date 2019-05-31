# Standard Modules

# Custom Modules
import switches          as     sws
from   hdl_signal        import HDL_Signal
from   malicious_counter import add_malicious_coal_counters

def generate_coalesced_counters(signals, num_mal_cntrs, dut_top_module):
	coal_counters = []

	# Find existing coalesced counters in the design
	for signal_name in signals.keys():

		# Check if signal is a flip-flop
		if signals[signal_name].isff and signal_name.startswith(dut_top_module):
			coal_counters.append(signals[signal_name])

	# Generate artificial coalesced counters
	if num_mal_cntrs > 0:
		print "Generating Malicious Coalesced Counters..."
		coal_counters = add_malicious_coal_counters(signals, coal_counters, num_mal_cntrs)

	return coal_counters
