# Standard Modules
import sys

# Custom Modules
import switches          as     sws
from   hdl_signal        import HDL_Signal
from   dist_signal       import SignalSlice, DistSignal
from   malicious_counter import add_malicious_dist_counters

# Builds out the dependencies for each signal passed in
# @TODO: Currently inefficient, re-computing a lot of things
def build_deps(source_sig, msb, lsb, ffs = [], seen = {}):
	# print "Building Dependencies..."
	# print "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
	# source_sig.debug_print()

	# If we've seen this exact slice before, continue
	if source_sig.sliced_fullname() in seen.keys():
		return ffs

	# Mark this signal as seen
	seen[source_sig.sliced_fullname()] = True

	# If its a FF or input, it is the end
	if (source_sig.isff or source_sig.isinput):
		
		# Make sure we aren't already inserted
		for ff in ffs:
			if ff.signal.sliced_fullname() == source_sig.sliced_fullname():
				return ffs

		ffs.append(SignalSlice(source_sig, msb, lsb))
		return ffs

	for c in source_sig.conn:
		build_deps(c.source_sig, c.source_msb, c.source_lsb, ffs, seen)

	return ffs
	
def generate_distributed_counters(signals, vcd, num_mal_cntrs, dut_top_module):
	dist_counters = {}

	for sig_name, sig in signals.iteritems():
		
		# Compute Dependencies
		ffs = build_deps(sig, sig.msb, sig.lsb, [], {})
		
		# print "----------------------------------------------------------"
		# print "Dependencies:"
		# print "	%s:" % (sig)
		# for ff in ffs:
		# 	print "		%s" % (ff)
		# print "=========================================================="
		# print

		# No flip-flops found influencing signal
		if len(ffs) == 0:
			continue

		# Only 1 flip-flop found --> ignore if it is itself
		# @TODO --> check that ignore is only for self, not all signals.keys()
		if len(ffs) == 1:
			if ffs[0].signal.fullname() in signals.keys():
				continue

		# Num flip-flops found is greater than 1 --> counter is distributed
		ffs.sort(key=lambda x: (x.signal.fullname(), x.lsb, x.msb)) # sort flip-flops alphabetically by name and msb:lsb range

		# Verify distributed counter components have been simulated
		counter_slice_names    = []
		dist_counter_width     = 0
		dist_counter_simulated = True
		dist_counter_in_dut    = True

		for ff in ffs:

			# Build distributed counter name and size
			slice_str = "[%d:%d]" % (ff.msb, ff.lsb)
			counter_slice_names.append(ff.signal.fullname() + slice_str)
			
			# Udpate MSB
			dist_counter_width += ff.width()

			# Check if base counter is in DUT
			if not ff.signal.fullname().startswith(dut_top_module):
				dist_counter_in_dut = False
				break

			# Check that base counter is simulated by test bench
			if not ff.signal.is_simulated():
				dist_counter_simulated = False
		
		# Create distributed counter signal name
		dist_counter_name = '{' + ','.join(counter_slice_names) + '}'

		# Check if dist counter is outside DUT
		if not dist_counter_in_dut:
			continue	

		# Check that dist_counter has not already been generated
		if dist_counter_name not in dist_counters:
			
			# Create new DistSignal object for distributed counter	
			dist_counter = DistSignal(dist_counter_name, dist_counter_width - 1, dist_counter_simulated, ffs)

			# Append dist_counter to list
			dist_counters[dist_counter_name] = dist_counter

	# Generate artificial coalesced counters
	if num_mal_cntrs > 0:
		print "Generating Malicious Distributed Counters..."
		dist_counters = add_malicious_dist_counters(signals, dist_counters, num_mal_cntrs)

	return dist_counters
