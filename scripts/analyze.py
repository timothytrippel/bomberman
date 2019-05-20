#!/usr/bin/env python

# Standard Modules
import re
import sys
import time
import pprint
import json

# Custom Modules
from hdl_signal          import HDL_Signal
from connection          import Connection
from parse_dot           import parse_file
from Verilog_VCD         import parse_vcd, get_timescale
from distributed_counter import generate_distributed_counters
from coalesced_counter   import generate_coalesced_counters

# Global Switches
global DEBUG_PRINTS

def calculate_and_print_time(start_time, end_time):
	hours, rem       = divmod(end_time - start_time, 3600)
	minutes, seconds = divmod(rem, 60)
	print "Execution Time:", "{:0>2}:{:0>2}:{:05.2f}".format(int(hours), int(minutes), seconds)

def update_signals_with_vcd(signals, vcd):
	for vcd_signal_name in vcd.keys():
		# Check that VCD signal name in signals dict
		assert vcd_signal_name in signals and "ERROR: VCD signal not in dot graph."
		
		# Check signal simulation info not already loaded
		assert signals[vcd_signal_name].tb_covered == False

		# Load signal simulation data
		signals[vcd_signal_name].add_vcd_sim(vcd[vcd_signal_name])

def classify_counters(counters, constants, malicious, skipped):
	for counter in counters:
		# Check that counter has been simulated by TB
		if not counter.tb_covered:
			print "WARNING: counter (%s) not exercised by test bench... skipping." % (counter.name)
			skipped[counter.name] = True
			continue

		# Compute number of simulated unique counter values
		counter_value_set = set(counter.time_values.values())

		# Compute number of possible unique counter values
		max_possible_values = 2 ** counter.width
		print "Values Seen:", len(counter_value_set) 
		print "Possible Values:", max_possible_values

		# Classify counter as a counter or malicious
		if len(counter_value_set) == 1:
			if counter.name not in constants:
				print "Constant: " + counter.name
				constants[counter.name] = True
		elif len(counter_value_set) < max_possible_values:
			if counter.name not in malicious:
				print "Possible Malicious Symbol: " + counter.name
				malicious[counter.name] = True

	return constants, malicious, skipped

def main():
	##
	# Set Program Switches
	##
	DEBUG_PRINTS = True

	##
	# Check argv
	##
	if (len(sys.argv) != 4):
		print "Usage: ./analyze.py <input dot file> <input vcd file> <output json file>"
		sys.exit(-1)

	##
	# Start Overall Timer
	##
	overall_start_time = time.time()

	##
	# Input Files
	##
	dot_file  = sys.argv[1]
	vcd_file  = sys.argv[2]
	json_file = sys.argv[3]

	##
	# Parse dot file
	##
	print "-------------------------------------------------"
	print "Parsing Dot File..."
	start_time = time.time()
	signals    = parse_file(dot_file) 
	end_time   = time.time()
	calculate_and_print_time(start_time, end_time)

	##
	# Parse vcd file / Generate Coalesced Counters
	##
	print "-------------------------------------------------"
	print "Parsing VCD File..."
	start_time = time.time()
	vcd        = parse_vcd(vcd_file, types={"reg", "wire"})
	update_signals_with_vcd(signals, vcd)
	timescale_str = get_timescale()
	timescale = timescale_str.rstrip('fpnums')
	print "Timescale:", timescale_str
	end_time   = time.time()
	calculate_and_print_time(start_time, end_time)

	##
	# Checked loaded Dot/VCD file data
	##
	if DEBUG_PRINTS:
		print "-------------------------------------------------"
		print "All Signals:"
		for signal_name in signals:
			signals[signal_name].debug_print()
	print

	##
	# Generate Distributed Counters
	##
	print "-------------------------------------------------"
	print "Generating Distributed Counters..."
	start_time    = time.time()
	dist_counters = generate_distributed_counters(signals, timescale)
	end_time      = time.time()
	print "Found " + str(len(dist_counters)) + " possible distributed counters."
	if DEBUG_PRINTS and dist_counters:
		for dist_counter in dist_counters:
			print "	Distributed Counter: %s (Size: %d)" % (dist_counter.name, dist_counter.width)
			dist_counter.debug_print()
	calculate_and_print_time(start_time, end_time)
	print

	##
	# Generate Coalesced Counters
	##
	print "-------------------------------------------------"
	print "Generating Coalesced Counters..."
	start_time    = time.time()
	coal_counters = generate_coalesced_counters(signals)
	end_time      = time.time()
	print "Found " + str(len(coal_counters)) + " possible coalesced counters."
	if DEBUG_PRINTS and coal_counters:
		for counter in coal_counters:
			print "	Coalesced Counter: %s (Size: %d)" % (counter.name, counter.width)
			counter.debug_print()
	calculate_and_print_time(start_time, end_time)
	print

	##
	# Analyze Distributed Counters
	##
	print "-------------------------------------------------"
	print "Finding malicious distributed signals..."
	start_time = time.time()
	dist_constants, dist_malicious, dist_skipped = classify_counters(dist_counters, {}, {}, {})
	end_time = time.time()
	calculate_and_print_time(start_time, end_time)
	print

	##
	# Analyze Coalesced Counters
	##
	print "-------------------------------------------------"
	print "Finding malicious coalesced signals..."
	start_time = time.time()
	coal_constants, coal_malicious, coal_skipped = classify_counters(coal_counters, {}, {}, {})
	end_time = time.time()
	calculate_and_print_time(start_time, end_time)
	print

	##
	# Report stats
	##
	print "-------------------------------------------------"
	print "Coalesced:"
	print "	# Possible:  " + str(len(coal_counters))
	print "	# Not Simd:  " + str(len(coal_skipped))
	print "	# Constants: " + str(len(coal_constants))
	print "	# Malicous:  " + str(len(coal_malicious))

	print "-------------------------------------------------"
	print "Distributed:"
	print "	# Possible:  " + str(len(dist_counters))
	print "	# Not Simd:  " + str(len(dist_skipped))
	print "	# Constants: " + str(len(dist_constants))
	print "	# Malicous:  " + str(len(dist_malicious))
	print "-------------------------------------------------"
	print "Analysis complete."
	print

	##
	# Write stats to JSON file
	##
	stats = {
		"coal_total"    : len(coal_counters),
		"coal_not_simd" : len(coal_skipped),
		"coal_constants": len(coal_constants),
		"coal_malicious": len(coal_malicious),
		"dist_total"    : len(dist_counters),
		"dist_not_simd" : len(dist_skipped),
		"dist_constants": len(dist_constants),
		"dist_malicious": len(dist_malicious)
	}
	with open(json_file, 'w') as jf:  
		json.dump(stats, jf)
	jf.close()

	##
	# Stop Overall Timer
	##
	overall_end_time = time.time()
	calculate_and_print_time(overall_start_time, overall_end_time)

if __name__== "__main__":
	main()
