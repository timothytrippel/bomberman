#!/usr/bin/env python

# Standard Modules
import re
import sys
import time
import pprint
import json

# Custom Modules
import switches            as     sws
from   hdl_signal          import HDL_Signal
from   connection          import Connection
from   parse_dot           import parse_file
from   Verilog_VCD         import parse_vcd, get_timescale, get_endtime
from   distributed_counter import generate_distributed_counters
from   coalesced_counter   import generate_coalesced_counters
from   compute_graph_stats import compute_max_fanin, compute_max_reg2reg_path

def calculate_and_print_time(start_time, end_time):
	hours, rem       = divmod(end_time - start_time, 3600)
	minutes, seconds = divmod(rem, 60)
	print "Execution Time:", "{:0>2}:{:0>2}:{:05.2f}".format(int(hours), int(minutes), seconds)

def export_stats_json(num_total, num_skipped, num_constants, num_malicious, filename):
	stats = {
		"total"     : num_total,
		"not_simd"  : num_skipped,
		"constants" : num_constants,
		"malicious" : num_malicious,
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
	for counter_name, counter in counters.items():
		counter_sizes.append(counter.width())
	return counter_sizes

def classify_counters(counter_type, signals, vcd, counters, start_time, time_limit, time_resolution, json_base_filename):

	# Counter Types
	skipped   = {}
	constants = {}
	malicious = {}
	benign    = {}

	# Flags
	no_malicious = False

	# Mark all counters as malicious
	for counter_name, counter in counters.items():

		# Check that counter has been simulated by TB
		if not counter.is_simulated():
			if sws.WARNINGS:
				print "WARNING: counter (%s) not exercised by test bench... skipping." % (counter.fullname())
			skipped[counter.fullname()] = True
		else:
			benign[counter_name]    = False
			malicious[counter_name] = set()

	timescale_str = '100ps'

	##
	# DEBUG mode: only one single time analysis
	##
	if sws.DEBUG:
		time_analysis_range = [time_limit]
		print "DEBUG MODE: analyzing entire simulation time interval:"

	##
	# NORMAL mode: iterate over simulation time intervals
	##
	else:
		time_analysis_range = range(start_time, time_limit, time_resolution)
		time_analysis_range.append(time_limit)

	for curr_time_limit in time_analysis_range:

		if sws.VERBOSE > 1:
			print "--------------------------------------------------------------------------------"
			print "Analyzing simulation at time interval:"
			print "[%d (*%s), %d (*%s)]" % \
			(start_time, timescale_str, curr_time_limit, timescale_str)

		# Iterate over (potentially) malicious counters
		for mal_counter_name in malicious:

			# Get HDL_Signal object representing counter
			counter = counters[mal_counter_name]

			# Print counter's name
			if sws.VERBOSE > 2:
				print counter.fullname()

			# Get next time value
			time, value = counter.get_time_value(vcd, curr_time_limit)

			# Iterate over time simulation time indices in range of interest
			while (time != None and value != None):

				# print "Sim Time:", time, "; Time Value:", value

				# Check if time value is valid
				if 'x' not in value:

					# Check if counter value already seen
					if value in malicious[mal_counter_name]:

						# NOT Malicious -> Continue
						if sws.VERBOSE > 2:
							print "Repeated Value (%s) --> Not Malicious" % (value)
							print

						# Mark counter as benign
						benign[mal_counter_name] = True
						break

					else:

						# Still possibly malicious: add value to set of simulated counter values
						malicious[mal_counter_name].add(value)

				# Get next time value
				time, value = counter.get_time_value(vcd, curr_time_limit)

			# Check if malicious counter was not already re-classified (i.e. a repeated value was seen)
			if not benign[mal_counter_name]:

				# Compute number of possible unique counter values
				max_possible_values = 2 ** counter.width()
				if sws.VERBOSE > 1:
					print "Values Seen/Possible: %d/%d" % (len(malicious[mal_counter_name]), max_possible_values)

				# Classify counter as a constant
				if len(malicious[mal_counter_name]) <= 1:
					if sws.VERBOSE > 1:
						print "Constant: " + mal_counter_name

					# Add to constants dict
					constants[mal_counter_name] = True

				# Classify counter as malicious
				elif len(malicious[mal_counter_name]) < max_possible_values:
					if sws.VERBOSE > 1:
						print "Possible Malicious Symbol: " + mal_counter_name

					# Remove from constants dict if it was previously a constant
					if mal_counter_name in constants:
						del constants[mal_counter_name]

				# Classify counter as NOT malicious
				else:
					if sws.VERBOSE > 2:
						print "Enumerated All Values"
					benign[mal_counter_name] = True

		# Remove non-malicious counters
		for mal_counter_name in benign:
			if benign[mal_counter_name]:
				if mal_counter_name in malicious:
					del malicious[mal_counter_name]
				if mal_counter_name in constants:
					del constants[mal_counter_name]

		# Create counter stats objects
		if sws.VERBOSE > 1:
			print "# Possible:  %d" % (len(counters))
			print "# Not Simd:  %d" % (len(skipped))
			print "# Constants: %d" % (len(constants))
			print "# Malicious: %d" % (len(malicious) - len(constants))

		# Save counter stats to JSON file
		json_filename = json_base_filename + "." + counter_type + "." + str(curr_time_limit) + ".json"
		export_stats_json(len(counters), len(skipped), len(constants), len(malicious) - len(constants), json_filename)

		# Check if hit 0 malicious counters
		if not no_malicious and len(malicious) == 0:
			no_malicious = True
			print "\nNo more malicious counters detected at: %d" % (curr_time_limit)

	# Print remaining malicious signals/constants
	print
	print "Malicious Signals (%d):" % (len(malicious) - len(constants))
	for mal_counter_name in malicious:
		if mal_counter_name not in constants:
			print "	%s" % (mal_counter_name)
	print
	print "Constants (%d):" % (len(constants))
	for const_counter_name in constants:
		print "	%s" % (const_counter_name)
	print

def analyze_counters(signals, vcd, coal_counters, dist_counters, start_time, time_limit, time_resolution, json_base_filename):

	##
	# Analyze Coalesced Counters
	##
	print
	print "--------------------------------------------------------------------------------"
	print "Finding malicious coalesced signals..."
	task_start_time    = time.time()
	classify_counters("coal", signals, vcd, coal_counters, start_time, time_limit, time_resolution, json_base_filename)
	task_end_time      = time.time()
	calculate_and_print_time(task_start_time, task_end_time)
	print

	##
	# Analyze Distributed Counters
	##
	print "--------------------------------------------------------------------------------"
	print "Finding malicious distributed signals..."
	task_start_time    = time.time()
	classify_counters("dist", signals, vcd, dist_counters, start_time, time_limit, time_resolution, json_base_filename)
	task_end_time      = time.time()
	calculate_and_print_time(task_start_time, task_end_time)
	print

	print "--------------------------------------------------------------------------------"
	print "Analysis complete."
	print

def check_simulation_coverage(signals, dut_top_module):
	print "--------------------------------------------------------------------------------"
	print "Checking simulation coverage..."
	print "Flip-Flops/Inputs NOT exercised by testbench:"
	num_ff_simd   = 0
	num_total_ffs = 0
	for signal_name in signals:

		# Check that signal is in the DUT top module (i.e. not a test bench signal)
		if signal_name.startswith(dut_top_module):
			if not signals[signal_name].vcd_symbol and (signals[signal_name].isff or signals[signal_name].isinput):
				print "	", signal_name
			else:
				num_ff_simd += 1
			num_total_ffs += 1

	# Compute coverage percentage
	print "Num. FFs/Inputs Simd: ", num_ff_simd
	print "Num. Total FFs/Inputs:", num_total_ffs
	simulation_coverage_pct = (float(num_ff_simd) / float(num_total_ffs)) * 100.00
	print
	print "FF/Input Simulation Coverage: {:.2f}%	({:d}/{:d})".format(\
		simulation_coverage_pct, \
		num_ff_simd, \
		num_total_ffs)
	print

def main():
	##
	# Set Global Program Switches
	##

	# General Switches
	sws.VERBOSE  = 0
	sws.WARNINGS = False

	# DEBUG Switches
	sws.DEBUG        = False
	sws.DEBUG_PRINTS = False

	##
	# Check argv
	##
	if (len(sys.argv) != 11):
		print "Usage: ./analyze.py"
		print "	<start time>"
		print "	<time limit>"
		print "	<time resolution>"
		print "	<DUT top module>"
		print "	<num. malicious cntrs>"
		print "	<deterministic signal basename (for malicious simulation)>"
		print "	<non-deterministic signal basename (for malicious simulation)>"
		print "	<input dot file>"
		print "	<input vcd file>"
		print "	<output json file (basename)>"
		sys.exit(-1)

	##
	# Start Overall Timer
	##
	overall_start_time = time.time()

	##
	# Input Files
	##
	print "--------------------------------------------------------------------------------"
	print "Loading Configuration Inputs..."
	start_time         = int(sys.argv[1])
	time_limit         = int(sys.argv[2])
	time_resolution    = int(sys.argv[3])
	dut_top_module     = sys.argv[4]
	num_mal_cntrs      = int(sys.argv[5])
	mal_d_sig_basename = sys.argv[6]
	mal_n_sig_basename = sys.argv[7]
	dot_file           = sys.argv[8]
	vcd_file           = sys.argv[9]
	json_base_filename = sys.argv[10]
	print
	print "Start Time:                 ", start_time
	print "Time Limit:                 ", time_limit
	print "Time Resolution:            ", time_resolution
	print "DUT Top Module:             ", dut_top_module
	print "Num. Malicious Cntrs:       ", num_mal_cntrs
	print "Deter. Signal Basename:     ", mal_d_sig_basename
	print "Nondeter. Signal Basename:  ", mal_n_sig_basename
	print "Num. Malicious Cntrs:       ", num_mal_cntrs
	print "DOT File:                   ", dot_file
	print "VCD File:                   ", vcd_file
	print "(Output) JSON Base Filename:", json_base_filename

	##
	# Load DOT file
	##
	print "--------------------------------------------------------------------------------"
	print "Loading Dot File..."
	task_start_time = time.time()
	signals         = parse_file(dot_file)
	task_end_time   = time.time()
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Compute Graph Stats
	##
	print "--------------------------------------------------------------------------------"
	print "Computing Fan-in Stats..."
        compute_max_fanin(signals, dut_top_module, json_base_filename + ".fanin.json")

	print "--------------------------------------------------------------------------------"
	print "Computing Reg2Reg Path Length Stats..."
        compute_max_reg2reg_path(signals, dut_top_module, json_base_filename + ".reg2reg.json")

	##
	# Load VCD file
	##
	print "--------------------------------------------------------------------------------"
	print "Loading VCD File..."
	print
	task_start_time = time.time()

	# Get VCD data
	vcd, signals = parse_vcd(vcd_file, signals, types={"reg", "wire", "integer"})

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
	# Check inputs
	##

	# Check simulation coverage
	check_simulation_coverage(signals, dut_top_module)

	# Check time limits
	print "--------------------------------------------------------------------------------"
	print "Checking time limits..."
	if time_limit == -1:
		time_limit = sim_end_time
	elif time_limit < -1:
		print "ERROR: time limit cannot be negative."
		print "Exception is -1, which indicates entire simulation time."
		sys.exit(1)

	# Print Simulation Time Settings
	print
	print "Start Time:      %d" % (start_time)
	print "Time Limit:      %d" % (time_limit)
	print "Time Resolution: %d" % (time_resolution)

	# Checked loaded Dot/VCD file data
	##
	if sws.DEBUG_PRINTS:
		print "--------------------------------------------------------------------------------"
		print "All Signals:"
		for signal_name in signals:
			signals[signal_name].debug_print()
	print

	##
	# Identify Coalesced Counters
	##
	print "--------------------------------------------------------------------------------"
	print "Identifying Coalesced Counter Candidates..."
	task_start_time = time.time()
	coal_counters   = generate_coalesced_counters(signals, vcd, num_mal_cntrs, dut_top_module, mal_d_sig_basename, mal_n_sig_basename)
	task_end_time   = time.time()
	print
	print "Found " + str(len(coal_counters)) + " possible coalesced counters."
	if sws.DEBUG_PRINTS and coal_counters:
		for counter in coal_counters:
			print "	Coalesced Counter: %s (Size: %d)" % (counter.fullname(), counter.width)
			counter.debug_print()
	coal_counter_sizes = get_counter_sizes(coal_counters)
	print
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Identify Distributed Counters
	##
	print "--------------------------------------------------------------------------------"
	print "Identifying Distributed Counter Candidates..."
	task_start_time = time.time()
	dist_counters   = generate_distributed_counters(signals, vcd, num_mal_cntrs, dut_top_module, mal_d_sig_basename, mal_n_sig_basename)
	task_end_time   = time.time()
	print
	print "Found " + str(len(dist_counters)) + " possible distributed counters."
	if sws.DEBUG_PRINTS and dist_counters:
		for dist_counter in dist_counters:
			print "	Distributed Counter: %s (Size: %d)" % (dist_counter.fullname(), dist_counter.width)
			dist_counter.debug_print()
	dist_counter_sizes = get_counter_sizes(dist_counters)
	print
	calculate_and_print_time(task_start_time, task_end_time)

	##
	# Write counter sizes to JSON file
	##
	export_sizes_json(coal_counter_sizes, dist_counter_sizes, json_base_filename + ".sizes.json")

	##
	# Classify counter behaviors
	##
	analyze_counters(signals, vcd, coal_counters, dist_counters, start_time, time_limit, time_resolution, json_base_filename)

	##
	# Stop Overall Timer
	##
	overall_end_time = time.time()
	calculate_and_print_time(overall_start_time, overall_end_time)

if __name__== "__main__":
	main()
