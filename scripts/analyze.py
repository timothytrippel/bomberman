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
from Verilog_VCD         import parse_vcd, get_timescale, get_endtime
from distributed_counter import generate_distributed_counters
from coalesced_counter   import generate_coalesced_counters
from counter_stats       import CounterStats

def calculate_and_print_time(start_time, end_time):
	hours, rem       = divmod(end_time - start_time, 3600)
	minutes, seconds = divmod(rem, 60)
	print "Execution Time:", "{:0>2}:{:0>2}:{:05.2f}".format(int(hours), int(minutes), seconds)

def export_stats_json(coal_counter_stats, dist_counter_stats, filename):
	stats = {
		"coal_total"     : coal_counter_stats.num_total,
		"coal_not_simd"  : coal_counter_stats.num_not_simd,
		"coal_constants" : coal_counter_stats.num_consts,
		"coal_malicious" : coal_counter_stats.num_mal,
		"dist_total"     : dist_counter_stats.num_total,
		"dist_not_simd"  : dist_counter_stats.num_not_simd,
		"dist_constants" : dist_counter_stats.num_consts,
		"dist_malicious" : dist_counter_stats.num_mal
	}
	with open(filename, 'w') as jf:  
		json.dump(stats, jf)
	jf.close()

def export_sizes_json(coal_counter_sizes, dist_counter_sizes, filename):
	stats = {
		"coal_sizes" : coal_counter_sizes,
		"dist_sizes" : dist_counter_sizes
	}
	with open(filename, 'w') as jf:  
		json.dump(stats, jf)
	jf.close()

def get_counter_sizes(counters):
	counter_sizes = []
	for counter in counters:
		counter_sizes.append(counter.width)
	return counter_sizes

def update_signals_with_vcd(signals, vcd):
	for vcd_signal_name in vcd.keys():

		# Check that VCD signal name in signals dict
		assert vcd_signal_name in signals and "ERROR: VCD signal not in dot graph."
		
		# Check signal simulation info not already loaded
		assert signals[vcd_signal_name].tb_covered == False

		# Load signal simulation data to only one signal object
		signals[vcd_signal_name].add_vcd_sim(vcd[vcd_signal_name])

		# Load reference to all signal objects on same net
		for net_dict in vcd[vcd_signal_name]['nets']:
			net_fullname = net_dict['hier'] + '.' + net_dict['name'].split('[')[0]
			
			if net_fullname in signals:
				signals[net_fullname].ref_hierarchy = signals[vcd_signal_name].hierarchy
				signals[net_fullname].tb_covered    = True
			else:
				print "ERROR: VCD signal not in dot graph."
				sys.exit(1)

def classify_counters(counter_type, signals, counters, constants, malicious, skipped, time_limit):
	for counter in counters:

		# Check that counter has been simulated by TB
		if not counter.tb_covered:
			if WARNINGS:
				print "WARNING: counter (%s) not exercised by test bench... skipping." % (counter.name)
			skipped[counter.name] = True
			continue

		# Compute number of simulated unique counter values in time interval
		counter_value_set = set()
		counter_sim_times = counter.get_sorted_update_times(signals)
		
		# Reset repested value flag
		repeated_value = False

		for sim_time in counter_sim_times:
			if sim_time <= time_limit:
				if 'x' not in counter.get_time_value(signals, sim_time):
					# Check if counter value already seen
					if counter.get_time_value(signals, sim_time) in counter_value_set:

						# NOT Malicious -> Continue
						if VERBOSE: 
							print "Repeated Value (%s) --> Not Malicious" % (counter.get_time_value(signals, sim_time))
						
						# Set repeated value flag
						repeated_value = True
						break

					else:
						# print counter.get_time_value(signals, sim_time)
						counter_value_set.add(counter.get_time_value(signals, sim_time))
			else:
				break

		# Check if repeated value
		if repeated_value:
			continue

		# Compute number of possible unique counter values
		max_possible_values = 2 ** counter.width
		if VERBOSE:
			print "Values Seen/Possible: %d/%d" % (len(counter_value_set), max_possible_values)

		# Classify counter as a counter as a constant
		if len(counter_value_set) == 1:
			if counter.name not in constants:
				if VERBOSE: 
					print "Constant: " + counter.name
				constants[counter.name] = True

		# Classify counter as a counter as malicious
		if len(counter_value_set) < max_possible_values:
			if counter.name not in malicious:
				if VERBOSE: 
					print "Possible Malicious Symbol: " + counter.name
				malicious[counter.name] = True

	return CounterStats(counter_type, counters, skipped, constants, malicious)

def analyze_counters(signals, coal_counters, dist_counters, curr_time_limit):
	
	##
	# Analyze Coalesced Counters
	##
	print
	print "Finding malicious coalesced signals..."
	task_start_time    = time.time()
	coal_counter_stats = classify_counters("Coalesced", signals, coal_counters, {}, {}, {}, curr_time_limit)
	task_end_time      = time.time()
	calculate_and_print_time(task_start_time, task_end_time)
	print

	# ##
	# # Analyze Distributed Counters
	# ##
	# print
	# print "Finding malicious distributed signals..."
	# task_start_time    = time.time()
	# dist_counter_stats = classify_counters("Distributed", signals, dist_counters, {}, {}, {}, curr_time_limit)
	# task_end_time      = time.time()
	# calculate_and_print_time(task_start_time, task_end_time)
	# print

	# ##
	# # Report stats
	# ##
	# coal_counter_stats.print_stats()
	# dist_counter_stats.print_stats()

	# ##
	# # Write stats to JSON file
	# ##
	# json_filename = json_base_filename + "." + str(curr_time_limit) + ".json"
	# export_stats_json(coal_counter_stats, dist_counter_stats, json_filename)

	print "Analysis complete."
	print

def main():
	
	##
	# Set Global Program Switches
	##
	global DEBUG
	global DEBUG_PRINTS
	global VERBOSE
	global WARNINGS
	DEBUG        = True
	DEBUG_PRINTS = False
	VERBOSE      = False
	WARNINGS     = False

	##
	# Check argv
	##
	if (len(sys.argv) != 7):
		print "Usage: ./analyze.py \
			<start_time> \
			<time_limit> \
			<time_resolution> \
			<input dot file> \
			<input vcd file> \
			<output json file (basename)>"
		sys.exit(-1)

	##
	# Start Overall Timer
	##
	overall_start_time = time.time()

	##
	# Input Files
	##
	start_time         = int(sys.argv[1])
	time_limit         = int(sys.argv[2])
	time_resolution    = int(sys.argv[3])
	dot_file           = sys.argv[4]
	vcd_file           = sys.argv[5]
	json_base_filename = sys.argv[6]

	##
	# Parse dot file
	##
	print "-------------------------------------------------"
	print "Parsing Dot File..."
	task_start_time = time.time()
	signals         = parse_file(dot_file) 
	task_end_time   = time.time()
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Parse vcd file / Generate Coalesced Counters
	##
	print "-------------------------------------------------"
	print "Parsing VCD File..."
	print
	task_start_time = time.time()

	# Get VCD data
	vcd = parse_vcd(vcd_file, types={"reg", "wire"})
	update_signals_with_vcd(signals, vcd)

	# Get Timescale Info
	timescale_str   = get_timescale()
	timescale_units = re.sub(r'\d+', '', timescale_str)
	timescale_val   = int(timescale_str.rstrip('fpnums'))
	print "Timescale:           %d (%s)" % (timescale_val, timescale_units)

	# Get Simulation Time
	sim_end_time        = int(get_endtime())
	scaled_sim_end_time = sim_end_time * timescale_val
	print "Simulation End Time: %d (%s)" % (scaled_sim_end_time, timescale_units)
	task_end_time = time.time()
	print
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Check time limits
	##
	print "-------------------------------------------------"
	print "Checking time limits..."
	print
	if time_limit == -1:
		time_limit = sim_end_time
	elif time_limit < -1:  
		print "ERROR: time limit cannot be negative."
		print "Exception is -1, which indicates entire simulation time."
		sys.exit(1)

	# Print Simulation Time Settings
	print "Start Time:      %d" % (start_time)
	print "Time Limit:      %d" % (time_limit)
	print "Time Resolution: %d" % (time_resolution)

	# Checked loaded Dot/VCD file data
	##
	if DEBUG_PRINTS:
		print "-------------------------------------------------"
		print "All Signals:"
		for signal_name in signals:
			signals[signal_name].debug_print(signals)
	print

	##
	# Generate Distributed Counters
	##
	print "-------------------------------------------------"
	print "Generating Distributed Counters..."
	print
	task_start_time = time.time()
	dist_counters   = generate_distributed_counters(signals)
	task_end_time   = time.time()
	print "Found " + str(len(dist_counters)) + " possible distributed counters."
	if DEBUG_PRINTS and dist_counters:
		for dist_counter in dist_counters:
			print "	Distributed Counter: %s (Size: %d)" % (dist_counter.name, dist_counter.width)
			dist_counter.debug_print(signals)
	dist_counter_sizes = get_counter_sizes(dist_counters)
	print
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Generate Coalesced Counters
	##
	print "-------------------------------------------------"
	print "Generating Coalesced Counters..."
	print
	task_start_time = time.time()
	coal_counters   = generate_coalesced_counters(signals)
	task_end_time   = time.time()
	print "Found " + str(len(coal_counters)) + " possible coalesced counters."
	if DEBUG_PRINTS and coal_counters:
		for counter in coal_counters:
			print "	Coalesced Counter: %s (Size: %d)" % (counter.name, counter.width)
			counter.debug_print(signals)
	coal_counter_sizes = get_counter_sizes(coal_counters)
	print
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Write counter sizes to JSON file
	##
	export_sizes_json(coal_counter_sizes, dist_counter_sizes, json_base_filename + ".sizes.json")

	##
	# DEBUG mode: only one single time analysis
	##
	if DEBUG:

		print "-------------------------------------------------"
		print "DEBUG MODE: analyzing entire simulation time interval:"
		print "[%d (*%s), %d (*%s)]" % \
		(start_time, timescale_str, sim_end_time, timescale_str)

		# Analyze counters in the design
		analyze_counters(signals, coal_counters, dist_counters, sim_end_time)

	##
	# NORMAL mode: iterate over simulation time intervals
	##
	else:

		for curr_time_limit in range(start_time, time_limit, time_resolution):

			print "-------------------------------------------------"
			print "Analyzing simulation at time interval:"
			print "[%d (*%s), %d (*%s)]" % \
			(start_time, timescale_str, curr_time_limit, timescale_str)

			# Analyze counters in the design
			analyze_counters(signals, coal_counters, dist_counters, curr_time_limit)

	##
	# Stop Overall Timer
	##
	overall_end_time = time.time()
	calculate_and_print_time(overall_start_time, overall_end_time)

if __name__== "__main__":
	main()
