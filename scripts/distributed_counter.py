# Standard Modules
import sys

# Custom Modules
import switches          as     sws
from   hdl_signal        import HDL_Signal
from   malicious_counter import add_malicious_dist_counters

class SignalSlice:
	def __init__(self, signal, msb, lsb):
		self.signal = signal
		self.msb    = msb
		self.lsb    = lsb

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

def add_time_value(signals, dist_counter, source_signal, msb, lsb, time, values):
	# print "MSB: %d; LSB: %d; Time: %d; Values: %s" % (msb, lsb, time, values)
	if time in dist_counter.get_time_values(signals):
		dist_counter.append_time_value(time, values[source_signal.width - msb - 1: source_signal.width - lsb])
	else:
		dist_counter.set_time_value(time, values[source_signal.width - msb - 1: source_signal.width - lsb])
	# print "Width:", dist_counter.width, "; After:", dist_counter.get_time_value(signals, time)

def generate_distributed_counters(signals, num_mal_cntrs, dut_top_module):
	not_simd      = {}
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
		# @TODO-Tim --> check that ignore is only for self, not all signals.keys()
		if len(ffs) == 1:
			if ffs[0].signal.fullname() in signals.keys():
				continue

		# Num flip-flops found is greater than 1 --> counter is distributed
		ffs.sort(key=lambda x: (x.signal.fullname(), x.lsb, x.msb)) # sort flip-flops alphabetically by name and msb:lsb range

		# Build distributed counter signal name
		dist_counter_names     = []
		dist_counter_simulated = True
		dist_counter_in_dut    = True
		for ff in ffs:

			# Build distributed counter name
			slice_str = "[%d:%d]" % (ff.msb, ff.lsb)
			dist_counter_names.append(ff.signal.fullname() + slice_str)

			# Check that distributed counter is not comprised of un-simulated signals
			if not ff.signal.tb_covered and ff.signal.fullname().startswith(dut_top_module):
				dist_counter_simulated         = False
				if ff.signal.fullname() not in not_simd:
					if sws.WARNINGS:
						print "WARNING: " + ff.signal.fullname() + " not found in VCD file."
				not_simd[ff.signal.fullname()] = True
			# Check that distributed counter only contains signals in the DUT top module
			elif not ff.signal.tb_covered and not ff.signal.fullname().startswith(dut_top_module):
				dist_counter_in_dut = False
				break
	
		# Check if dist counter is outside DUT
		if not dist_counter_in_dut:
			continue	

		# Create new HDL_Signal object for distributed counter
		dist_counter_name = '{' + ','.join(dist_counter_names) + '}'
		dist_counter      = HDL_Signal(dist_counter_name, -1, 0, None)
		
		# Update tb_covered flag
		dist_counter.tb_covered = dist_counter_simulated

		# Check that dist_counter has not already been generated
		if dist_counter_name not in dist_counters:
			
			# Append dist_counter to list
			dist_counters[dist_counter_name] = dist_counter

		# Check if distributed signal was simd, if not, skip it
		if not dist_counter_simulated:
			continue

		# Get (sorted) list of time values of signals changing
		update_times = set()
		for ff in ffs:
			for time in signals[ff.signal.fullname()].get_update_times(signals):
				update_times.add(time)
		update_times = sorted(update_times)

		# Piece together simulated time values from dist. counter signals.
		for ff in ffs:
			source_signal = signals[ff.signal.fullname()]
			width         = ff.msb - ff.lsb + 1
			# print "REF Signal:"
			# print "	NAME:  %s" % (source_signal.fullname())
			# print "	MSB:   %d" % (ff.msb)
			# print "	LSB:   %d" % (ff.lsb)
			# print "	WIDTH: %d" % (width)
			# source_signal.debug_print_wtvs(signals)

			# Update counter width and msb
			dist_counter.width += width
			dist_counter.msb    = dist_counter.lsb + dist_counter.width - 1

			# Update counter time values
			for time in update_times:
				tv = source_signal.get_time_value(signals, time)
				add_time_value(signals, dist_counter, source_signal, ff.msb, ff.lsb, time, tv)

	# Generate artificial coalesced counters
	if num_mal_cntrs > 0:
		print "Generating Malicious Distributed Counters..."
		dist_counters = add_malicious_dist_counters(signals, dist_counters, num_mal_cntrs)

	return dist_counters
