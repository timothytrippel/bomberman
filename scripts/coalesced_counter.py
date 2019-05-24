# Standard Modules

# Custom Modules
from hdl_signal        import HDL_Signal
from malicious_counter import add_malicious_coal_counters

def generate_coalesced_counters(signals, add_malicious_cntrs=False):
	coal_counters = []

	# Find existing coalesced counters in the design
	for signal_name in signals.keys():

		# Check if signal is a flip-flop
		if signals[signal_name].isff:
			coal_counters.append(signals[signal_name])

	# Generate artificial coalesced counters
	if add_malicious_cntrs:
		print "Generating Malicious Coalesced Counters..."
		coal_counters = add_malicious_coal_counters(signals, coal_counters)

	return coal_counters
