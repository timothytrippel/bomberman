# Standard Modules

# Custom Modules

def generate_coalesced_counters(signals):
	coal_counters = []

	for signal_name in signals.keys():
		if signals[signal_name].isff:
			coal_counters.append(signals[signal_name])

	return coal_counters
