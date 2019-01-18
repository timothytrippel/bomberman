#!/usr/bin/env python
import sys
import re

from parse_dot import parse_file
from parse_dot import generate_distributed_counters

from Verilog_VCD import parse_vcd

global DEBUG_PRINTS

# Looks for unique values in a list
#
# If there is a repeated value we return 0
# Otherwise we return the number of items in the list
# @TODO: There must be an easier way to do this
def unique_values(l):
	seen = set()
	for item in l:
		if ''.join(item[1]) not in seen:
			# Necessary for 'x' or 'z' values
			if 'x' not in item[1] and 'y' not in item[1]:
				# New value, add to set
				seen.add(''.join(item[1]))
		else:
			# There was a repeat, return 0
			return 0
	return len(seen)

def merge_two_signals(sig1, sig2):
	new_str = ""
	assert len(sig1) == len(sig2)
	for i in range(0, len(sig1)):
		assert sig1[i] == 'x' or sig2[i] == 'x'
		if sig1[i] == 'x':
			new_str += sig2[i]
		else:
			new_str += sig1[i]
	return new_str

def exchange_sym_for_name_vcd(vcd):
	newvcd = {}
	for symbol, signal in vcd.iteritems():
		for net in signal["nets"]:
			name = net["hier"] + "." + net["name"].split("[")[0]
			signal["lsb"] = 0

			# If larger, extend
			if len(net["name"].split("[")) != 1:
				signal["lsb"] = int(net["name"].split("[")[1].split(":")[1].split("]")[0])
				# Zero Extend any shortened strings
				for i, _ in enumerate(signal['tv']):
					value = signal['tv'][i]
					size = int(net['size'])
					if (len(value[1]) != size):
						if (value[1][0] == 'x'):
							value = (value[0], ['x'] * (size - len(value[1])) + value[1])
						else:
							value = (value[0], ['0'] * (size - len(value[1])) + value[1])
					signal['tv'][i] = value
					assert len(value[1]) == size

			newvcd[name] = signal
	return newvcd

def update_signals_with_vcd(signals, vcd):
	# All simulated signals are possible coalesced counters
	coal_counters = []

	for vcd_signal in vcd.keys():
		# Check that VCD signal name in signals dict
		if vcd_signal not in signals:
			print signals
			print vcd_signal
			print "ERROR: VCD signal not in dot graph."
			sys.exit(1)
		else:
			# Check signal simulation info not already load
			assert signals[vcd_signal].tb_covered == False

			# Load signal simulation data
			signals[vcd_signal].add_vcd_sim(vcd[vcd_signal])
			coal_counters.append(vcd_signal)

	return coal_counters

# Check that all extracted signals in Dot file are also
# in the VCD file, i.e. they are exercised by the test bench.
def check_all_sigs_in_vcd(slices, vcd):
	for s in slices:
		if s.fullname() not in vcd.keys():
			print "WARNING: " + s.fullname() + " not in VCD file"
			return False
	return True

def debug_print_vcd(vcd):
	for signal in vcd.keys():
		print "Signal: %s" % (signal)
		print "	LSB:   %s" % (vcd[signal]['lsb'])
		print "	Nets:  %s" % (vcd[signal]['nets'])
		print "		Hier: %s" % (vcd[signal]['nets'][0]['hier'])
		print "		Name: %s" % (vcd[signal]['nets'][0]['name'])
		print "		Type: %s" % (vcd[signal]['nets'][0]['type'])
		print "		Size: %d" % (int(vcd[signal]['nets'][0]['size']))
		print "	Time-Values:"
		for tv in vcd[signal]['tv']:
			print "		%3d -- %s" % (tv[0], tv[1])

def main():
	##
	# Set Program Switches
	##
	DEBUG_PRINTS = True

	##
	# Check argv
	##
	if (len(sys.argv) != 3):
		print "Usage: ./analyze.py <dot file> <vcd file>"
		sys.exit(-1)

	##
	# Input Files
	##
	dot_file = sys.argv[1]
	vcd_file = sys.argv[2]

	##
	# Parse dot file
	##
	print "Parsing Dot File..."
	signals = parse_file(dot_file)
	if DEBUG_PRINTS:
		for signal_name in signals:
			signals[signal_name].debug_print()
	print 

	print "Generating Distributed Counters..."
	dist_counters = generate_distributed_counters(signals)
	print "Found " + str(len(dist_counters)) + " possible distributed counters:"
	if DEBUG_PRINTS:
		for counter in dist_counters:
			print "	Distributed Counter Size: %d" % len(counter)
			for signal in counter:
				print "		%s" % (signal)
	print

	##
	# Parse vcd file
	##
	print "Parsing VCD File..."
	vcd = parse_vcd(vcd_file, types={"reg", "wire"})
	vcd = exchange_sym_for_name_vcd(vcd)
	coal_counters = update_signals_with_vcd(signals, vcd)
	print "Found " + str(len(coal_counters)) + " possible coalesced counters:"
	if DEBUG_PRINTS:
		for counter in coal_counters:
			print "	Coalesced Counter: %s (Size: %d)" % (counter, signals[counter].width)
			# signals[counter.name].debug_print()
	print

if __name__== "__main__":
	main()


# ##
# # Check our data is the same as the VCD
# ##

# ##
# # Generate unsorted timevalue arrays
# ##
# progress             = 0
# num                  = 0
# num_signals          = 0
# num_malic_signals    = 0
# num_constant_signals = 0
# constants            = {}
# malicious            = {}

# print "Finding Malicious distributed signals..."
# for slices in counters:
# 	# Can only evaluate counters that have been simulated
# 	# @TODO-Tim: print warning that possible counter component,
# 	# 			 i.e. flip-flop, has not been simulated.
# 	if not check_all_sigs_in_vcd(slices, vcd):
# 		continue

# 	value_set = set()
# 	tvs       = []

# 	# Figure out width
# 	width        = 0
# 	num_signals += 1
# 	for s in slices:
# 		lsb = s.lsb
# 		msb = s.msb
# 		assert msb >= lsb

# 		data = {
# 			'start' : width,
# 			'width' : msb - lsb + 1,
# 			'tv'    : vcd[s.fullname()]["tv"],
# 			'it'    : 0,
# 			'msb'   : msb,
# 			'lsb'   : lsb,
# 			'actual_lsb' : vcd[s.fullname()]["lsb"]
# 		}

# 		width += (msb - lsb) + 1
# 		tvs.append(data)

# 	value = ['x'] * width
	
# 	while len(tvs) > 0:
# 		assert len(value) == width
# 		lowest_time = sys.maxint
# 		next_values = []

# 		# Find the next signal(s) change in time
# 		for tv in tvs:
# 			assert len(tv['tv']) != tv['it']
# 			if tv['tv'][tv['it']][0] < lowest_time:
# 				next_values = [tv]
# 				lowest_time = tv['tv'][tv['it']][0]
# 			elif tv['tv'][tv['it']][0] == lowest_time:
# 				next_values.append(tv)

# 		for val in next_values:
# 			print val
# 		print
# 		# Match up bit indexes of FF set
# 		assert len(next_values) >= 1
# 		for tv in next_values:
# 			assert tv['start'] + tv['width'] <= width
# 			next_value = tv['tv'][tv['it']][1]
# 			next_value = next_value[len(next_value) - tv["msb"] - 1 + tv["actual_lsb"]:len(next_value) - tv["lsb"] + tv["actual_lsb"]]
# 			assert len(next_value) == tv["msb"] - tv["lsb"] + 1
# 			value[tv['start']:tv['start'] + tv['width']] = next_value
# 			tv['it'] += 1


# 		if 'x' not in value:
# 			if ''.join(value) in value_set:
# 				break
# 			value_set.add(''.join(value))

# 		tvs[:] = [tv for tv in tvs if len(tv['tv']) > tv['it']]

# 	if len(tvs) == 0:
# 		name = "{" + ", ".join(str(x) for x in slices) + "}"
# 		if len(value_set) == 1 and name not in constants:
# 			print "Constant: " + name
# 			constants[name] = True
# 			num_constant_signals += 1
# 		elif name not in malicious:
# 			print "Possible Malicious Symbol: " + name
# 			malicious[name] = True
# 			num_malic_signals += 1

# 	num += 1

# print "Finding malicious coalesed signals"
# for _, data in vcd.iteritems():
# 	print _
# 	# All regs and any created wires
# 	if not data["nets"][0]["type"] == "reg": 
# 		continue
# 	name = data["nets"][0]["hier"] + "." + data["nets"][0]["name"]
# 	num_signals += 1

# 	# Compute number of unique values simulated for a set of FFs
# 	num_unique_values = unique_values(data["tv"]) 

# 	# Compute max number of possible unique values
# 	max_num_unique_values = 2 ** int(data["nets"][0]["size"])

# 	# num_unique_values != 0 checks for a repeated value
# 	# num_unique_values != max_num_unique_values checks that all values were enumerated
# 	if num_unique_values != 0 and num_unique_values != max_num_unique_values:
# 		if num_unique_values == 1:
# 			if data["nets"][0]["hier"] == "":
# 				counter = counters[int(re.search('(\d+)$', data["nets"][0]["name"]).group(0))]
# 				name    = "{" + ", ".join(str(x) for x in counter) + "}"
# 			if name not in constants:
# 				print "Constant: " + name
# 				constants[name] = True
# 				num_constant_signals += 1
# 		else:
# 			if data["nets"][0]["hier"] == "":
# 				counter = counters[int(re.search('(\d+)$', data["nets"][0]["name"]).group(0))]
# 				name = "{" + ", ".join(str(x) for x in counter) + "}"
# 			if name not in malicious:
# 				print "Possible Malicious Symbol: " + name
# 				malicious[name] = True
# 				num_malic_signals += 1
# print "Malicous Marked: " + str(num_malic_signals)
# print "Constant Marked: " + str(num_constant_signals)
