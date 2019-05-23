# Standard Modules
import sys

# Custom Modules
from hdl_signal import HDL_Signal

# Builds out the dependencies for each signal passed in
# @TODO: Currently inefficient, re-computing a lot of things
def build_deps(sig, msb, lsb, ffs = [], seen = {}):
	# print "Building Dependencies..."
	# print "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
	# sig.debug_print()

	# If we've seen this exact slice before, continue
	if str(sig) + str(msb) + str(lsb) in seen.keys():
		return ffs

	# Mark this signal as seen
	seen[str(sig) + str(msb) + str(lsb)] = True

	# If its a FF or input, it is the end
	if (sig.isff or sig.isinput):
		# Make sure we aren't already inserted
		for ff in ffs:
			if str(ff) == str(sig):
				return ffs

		ffs.append(HDL_Signal(sig.fullname(), msb, lsb))
		return ffs

	for c in sig.conn:
		build_deps(c.sig, c.msb, c.lsb, ffs, seen)

	return ffs

def add_time_value(signals, dist_counter, hdl_signal, msb, lsb, time, values):
	# print "MSB: %d; LSB: %d; Time: %d; Values: %s" % (msb, lsb, time, values)
	if time in dist_counter.get_time_values(signals):
		dist_counter.append_time_value(time, values[hdl_signal.width - msb - 1: hdl_signal.width - lsb])
	else:
		dist_counter.set_time_value(time, values[hdl_signal.width - msb - 1: hdl_signal.width - lsb])

	# print "After:", dist_counter.get_time_value(signals, time)

def generate_distributed_counters(signals):
	seen = {}
	dist_counters = []

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
			if ffs[0].fullname() in signals.keys():
				continue

		# Num flip-flops found is greater than 1 --> counter is distributed
		ffs.sort(key=str) # sort flip-flops alphabetically by name and msb:lsb range

		# Create new HDL_Signal object for distributed counter
		dist_counter_name      = '{' + ','.join([str(ff) for ff in ffs]) + '}'
		dist_counter           = HDL_Signal(dist_counter_name, -1, 0)
		dist_counter_simulated = True
		
		# Get (sorted) list of time values of signals changing
		update_times = set()
		for source_signal in ffs:
			for time in signals[source_signal.fullname()].get_update_times(signals):
				update_times.add(time)
		update_times = sorted(update_times)

		# Piece together simulated time values from dist. counter signals.
		# Sort source signals by number of VCD time values
		# print "-------------------------------------------------"
		for source_signal in ffs:
			hdl_signal = signals[source_signal.fullname()]
			msb        = source_signal.msb
			lsb        = source_signal.lsb
			width      = msb - lsb + 1
			# print "REF Signal:"
			# print "	NAME:  %s" % (hdl_signal.name)
			# print "	MSB:   %d" % (msb)
			# print "	LSB:   %d" % (lsb)
			# print "	WIDTH: %d" % (width)
			# hdl_signal.debug_print(signals)

			# Update counter width and msb
			dist_counter.width += width
			dist_counter.msb    = dist_counter.lsb + dist_counter.width - 1

			# Check if source signal has been covered (simulated) by TB
			if not hdl_signal.check_signal_simulated():
				dist_counter_simulated = False
				break

			# Update counter time values
			for i in range(len(update_times)):

				# Get time
				time = update_times[i]

				# Indicator flag
				time_value_found = False

				# Check if time value exists in VCD file:
				if time in hdl_signal.get_time_values(signals):
					add_time_value(signals, dist_counter, hdl_signal, msb, lsb, time, hdl_signal.get_time_value(signals, time))
					time_value_found = True
				else:

					# Find last time a value was recorded
					j = i
					while j >= 0:

						# Get earlier time
						earlier_time = update_times[j]

						# Check if value recorded for an earlier time
						if earlier_time in hdl_signal.get_time_values(signals):
							add_time_value(signals, dist_counter, hdl_signal, msb, lsb, time, hdl_signal.get_time_value(signals, earlier_time))
							time_value_found = True
							break

						# Update (earlier) time index
						j -= 1

				# Check if time value found
				if not time_value_found:
					print "ERROR: unkown time value for signal (%s) at time (%d)" % (source_signal.fullname(), time)
					sys.exit(1) 

		# Update tb_covered flag
		dist_counter.tb_covered = dist_counter_simulated
		
		# Check that dist_counter has not already been generated
		if dist_counter_name not in seen:
			seen[dist_counter_name] = True
			
			# Append dist_counter to list
			dist_counters.append(dist_counter)

	return dist_counters
