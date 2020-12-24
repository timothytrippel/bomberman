#!/usr/bin/env python

# Copyright © 2019, Massachusetts Institute of Technology
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

import argparse
import json
import os
import re
import sys
import time

import switches as sws
from coalesced_counter import generate_coalesced_counters
from compute_graph_stats import compute_max_fanin, compute_max_reg2reg_path
from distributed_counter import generate_distributed_counters
from parse_dot import parse_file
from Verilog_VCD import get_endtime, get_timescale, parse_vcd

# ------------------------------------------------------------------------------
# Globals
# ------------------------------------------------------------------------------
TERMINAL_ROWS, TERMINAL_COLS = os.popen("stty size", "r").read().split()
LINE_SEP = "=" * int(TERMINAL_COLS)


def calculate_and_print_time(start_time, end_time):
  hours, rem = divmod(end_time - start_time, 3600)
  minutes, seconds = divmod(rem, 60)
  print("Execution Time:",
        "{:0>2}:{:0>2}:{:05.2f}".format(int(hours), int(minutes), seconds))


def export_stats_json(num_total, num_skipped, num_constants, num_malicious,
                      filename):
  stats = {
      "total": num_total,
      "not_simd": num_skipped,
      "constants": num_constants,
      "malicious": num_malicious,
  }
  with open(filename, "w") as jf:
    json.dump(stats, jf)
  jf.close()


def export_sizes_json(coal_counter_sizes, dist_counter_sizes, filename):
  stats = {"coal_sizes": coal_counter_sizes, "dist_sizes": dist_counter_sizes}
  with open(filename, "w") as jf:
    json.dump(stats, jf)
  jf.close()


def get_counter_sizes(counters):
  counter_sizes = []
  for counter_name, counter in counters.items():
    counter_sizes.append(counter.width())
  return counter_sizes


def classify_counters(counter_type, signals, vcd, counters, start_time,
                      time_limit, time_resolution, json_base_filename):

  # Counter Types
  skipped = {}
  constants = {}
  malicious = {}
  benign = {}

  # Flags
  no_malicious = False

  # Mark all counters as malicious
  for counter_name, counter in counters.items():

    # Check that counter has been simulated by TB
    if not counter.is_simulated():
      if sws.WARNINGS:
        print(
            "WARNING: counter (%s) not exercised by test bench... skipping." %
            (counter.fullname()))
      skipped[counter.fullname()] = True
    else:
      benign[counter_name] = False
      malicious[counter_name] = set()

  # TODO(timothytrippel): make timescale variable (read from VCD file)
  timescale_str = "100ps"

  # DEBUG mode: only one single time analysis
  if sws.DEBUG:
    time_analysis_range = [time_limit]
    print("DEBUG MODE: analyzing entire simulation time interval:")
  # NORMAL mode: iterate over simulation time intervals
  else:
    time_analysis_range = list(range(start_time, time_limit, time_resolution))
    time_analysis_range.append(time_limit)

  for curr_time_limit in time_analysis_range:
    if sws.VERBOSE > 1:
      print(LINE_SEP)
      print("Analyzing simulation at time interval:")
      print("[%d (*%s), %d (*%s)]" %
            (start_time, timescale_str, curr_time_limit, timescale_str))

    # Iterate over (potentially) malicious counters
    for mal_counter_name in malicious:

      # Get HDL_Signal object representing counter
      counter = counters[mal_counter_name]

      # Print counter's name
      if sws.VERBOSE > 2:
        print(counter.fullname())

      # Get next time value
      time, value = counter.get_time_value(vcd, curr_time_limit)

      # Iterate over time simulation time indices in range of interest
      while (time is not None and value is not None):
        # Check if time value is valid
        if "x" not in value:
          if value in malicious[mal_counter_name]:
            # mark BENIGN and continue
            if sws.VERBOSE > 2:
              print("Repeated Value (%s) --> Not Malicious\n" % (value))
            benign[mal_counter_name] = True
            break
          else:
            # Still possibly malicious: add value to set of simulated values
            malicious[mal_counter_name].add(value)
        time, value = counter.get_time_value(vcd, curr_time_limit)

      # Check if malicious counter was not already re-classified
      # (i.e. a repeated value was seen)
      if not benign[mal_counter_name]:
        # Compute number of possible unique counter values
        max_possible_values = 2**counter.width()
        if sws.VERBOSE > 1:
          print("Values Seen/Possible: %d/%d" %
                (len(malicious[mal_counter_name]), max_possible_values))

        # Classify counter as a constant
        if len(malicious[mal_counter_name]) <= 1:
          if sws.VERBOSE > 1:
            print("Constant: %s" % mal_counter_name)

          # Add to constants dict
          constants[mal_counter_name] = True

        # Classify counter as malicious
        elif len(malicious[mal_counter_name]) < max_possible_values:
          if sws.VERBOSE > 1:
            print("Possible Malicious Symbol: %s" % mal_counter_name)

          # Remove from constants dict if it was previously a constant
          if mal_counter_name in constants:
            del constants[mal_counter_name]

        # Classify counter as NOT malicious
        else:
          if sws.VERBOSE > 2:
            print("Enumerated All Values")
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
      print("# Possible:  %d" % (len(counters)))
      print("# Not Simd:  %d" % (len(skipped)))
      print("# Constants: %d" % (len(constants)))
      print("# Malicious: %d" % (len(malicious) - len(constants)))

    # Save counter stats to JSON file
    json_filename = json_base_filename + "." + counter_type + "." + str(
        curr_time_limit) + ".json"
    export_stats_json(len(counters), len(skipped), len(constants),
                      len(malicious) - len(constants), json_filename)

    # Check if hit 0 malicious counters
    if not no_malicious and len(malicious) == 0:
      no_malicious = True
      print("\nNo more malicious counters detected at: %d" % (curr_time_limit))

  # Print remaining malicious signals/constants
  print("\nMalicious Signals (%d):" % (len(malicious) - len(constants)))
  for mal_counter_name in malicious:
    if mal_counter_name not in constants:
      print("	%s" % (mal_counter_name))
  print("\nConstants (%d):" % (len(constants)))
  for const_counter_name in constants:
    print("	%s" % (const_counter_name))
  print()


def analyze_counters(signals, vcd, coal_counters, dist_counters, start_time,
                     time_limit, time_resolution, json_base_filename):

  # Analyze coalesced counters
  print()
  print(LINE_SEP)
  print("Finding malicious coalesced signals...")
  task_start_time = time.time()
  classify_counters("coal", signals, vcd, coal_counters, start_time,
                    time_limit, time_resolution, json_base_filename)
  task_end_time = time.time()
  calculate_and_print_time(task_start_time, task_end_time)
  print

  # Analyze distributed counters
  print(LINE_SEP)
  print("Finding malicious distributed signals...")
  task_start_time = time.time()
  classify_counters("dist", signals, vcd, dist_counters, start_time,
                    time_limit, time_resolution, json_base_filename)
  task_end_time = time.time()
  calculate_and_print_time(task_start_time, task_end_time)
  print()
  print(LINE_SEP)
  print("Analysis complete.\n")


def check_simulation_coverage(signals, dut_top_module):
  print(LINE_SEP)
  print("Checking simulation coverage...")
  print("Flip-Flops/Inputs NOT exercised by testbench:")
  num_ff_simd = 0
  num_total_ffs = 0
  for signal_name in signals:

    # Check that signal is in the DUT top module (i.e. not a test bench signal)
    if signal_name.startswith(dut_top_module):
      if not signals[signal_name].vcd_symbol and (
          signals[signal_name].isff or signals[signal_name].isinput):
        print("\t", signal_name)
      else:
        num_ff_simd += 1
      num_total_ffs += 1

  # Compute coverage percentage
  print("Num. FFs/Inputs Simd: ", num_ff_simd)
  print("Num. Total FFs/Inputs:", num_total_ffs)
  simulation_coverage_pct = (float(num_ff_simd) /
                             float(num_total_ffs)) * 100.00
  print("\nFF/Input Simulation Coverage: {:.2f}%	({:d}/{:d})\n".format(
      simulation_coverage_pct, num_ff_simd, num_total_ffs))


def main():
  # Set General Switches
  sws.VERBOSE = 0
  sws.WARNINGS = False

  # Set Debug switches
  sws.DEBUG = False
  sws.DEBUG_PRINTS = False

  # CMD line args
  parser = argparse.ArgumentParser(description="Bomberman")
  parser.add_argument("start_time",
                      type=int,
                      help="Simulation time to start Bomberman analysis.")
  parser.add_argument("time_limit",
                      type=int,
                      help="Simulation time to stop Bomberman analysis.")
  parser.add_argument(
      "time_resolution",
      type=int,
      help="Simulation time step between Bomberman analysis checks.")
  parser.add_argument("dut_top_module", help="Name of toplevel DUT module.")
  parser.add_argument("num_mal_cntrs",
                      type=int,
                      help="Number of malicious SSCs to implant.")
  parser.add_argument(
      "mal_d_sig_basename",
      help="Basename of increment signal to build periodic TTTs from.")
  parser.add_argument(
      "mal_n_sig_basename",
      help="Basename of increment signal to build sporadic TTTs from.")
  parser.add_argument("dot_file",
                      help="Graphviz dot file describing circuit DFG.")
  parser.add_argument("vcd_file", help="VCD file of simulation results.")
  parser.add_argument("json_base_filename", help="Output JSON file basename.")

  # Start analysis timer
  overall_start_time = time.time()

  # Print configurations
  print(LINE_SEP)
  print("Loading configuration inputs...")
  args = parser.parse_args()
  print(LINE_SEP)
  print("Start Time:                 ", args.start_time)
  print("Time Limit:                 ", args.time_limit)
  print("Time Resolution:            ", args.time_resolution)
  print("DUT Top Module:             ", args.dut_top_module)
  print("Num. Malicious Cntrs:       ", args.num_mal_cntrs)
  print("Deter. Signal Basename:     ", args.mal_d_sig_basename)
  print("Nondeter. Signal Basename:  ", args.mal_n_sig_basename)
  print("Num. Malicious Cntrs:       ", args.num_mal_cntrs)
  print("DOT File:                   ", args.dot_file)
  print("VCD File:                   ", args.vcd_file)
  print("(Output) JSON Base Filename:", args.json_base_filename)

  # Load circuit DFG file
  print(LINE_SEP)
  print("Loading dot file...")
  print(LINE_SEP)
  task_start_time = time.time()
  signals = parse_file(args.dot_file)
  task_end_time = time.time()
  calculate_and_print_time(task_start_time, task_end_time)

  # Compute DFG stats
  print(LINE_SEP)
  print("Computing DFG stats...")
  print(LINE_SEP)
  compute_max_fanin(signals, args.dut_top_module,
                    args.json_base_filename + ".fanin.json")

  print(LINE_SEP)
  print("Computing reg2reg path length stats...")
  compute_max_reg2reg_path(signals, args.dut_top_module,
                           args.json_base_filename + ".reg2reg.json")

  # Load VCD file
  print(LINE_SEP)
  print("Loading VCD file...")
  task_start_time = time.time()
  vcd, signals = parse_vcd(args.vcd_file,
                           signals,
                           types={"reg", "wire", "integer"})
  timescale_str = get_timescale()
  timescale_units = re.sub(r"\d+", "", timescale_str)
  timescale_val = int(timescale_str.rstrip("fpnums"))
  print("Timescale:           %d (%s)" % (timescale_val, timescale_units))

  # Get Simulation Time
  sim_end_time = int(get_endtime())
  scaled_sim_end_time = sim_end_time * timescale_val
  print("Simulation End Time: %d (%s)" %
        (scaled_sim_end_time, timescale_units))
  task_end_time = time.time()
  print()
  calculate_and_print_time(task_start_time, task_end_time)

  # Check inputs
  check_simulation_coverage(signals, args.dut_top_module)
  print(LINE_SEP)
  print("Checking time limits...")
  if args.time_limit == -1:
    args.time_limit = sim_end_time
  elif args.time_limit < -1:
    print("ERROR: time limit cannot be negative.")
    print("Exception is -1, which indicates entire simulation time.")
    sys.exit(1)
  print()
  print("Start Time:      %d" % (args.start_time))
  print("Time Limit:      %d" % (args.time_limit))
  print("Time Resolution: %d" % (args.time_resolution))

  # Check loaded dot/VCD file data
  if sws.DEBUG_PRINTS:
    print(LINE_SEP)
    print("All Signals:")
    for signal_name in signals:
      signals[signal_name].debug_print()
  print()

  # Identify coalesced counters
  print(LINE_SEP)
  print("Identifying coalesced counter candidates...")
  task_start_time = time.time()
  coal_counters = generate_coalesced_counters(signals, vcd, args.num_mal_cntrs,
                                              args.dut_top_module,
                                              args.mal_d_sig_basename,
                                              args.mal_n_sig_basename)
  task_end_time = time.time()
  print("\nFound %s possible coalesced counters." % str(len(coal_counters)))
  if sws.DEBUG_PRINTS and coal_counters:
    for counter in coal_counters:
      print("\tCoalesced Counter: %s (Size: %d)" %
            (counter.fullname(), counter.width))
      counter.debug_print()
  coal_counter_sizes = get_counter_sizes(coal_counters)
  print()
  calculate_and_print_time(task_start_time, task_end_time)

  # Identify distributed counters
  print(LINE_SEP)
  print("Identifying distributed counter candidates...")
  task_start_time = time.time()
  dist_counters = generate_distributed_counters(signals, vcd,
                                                args.num_mal_cntrs,
                                                args.dut_top_module,
                                                args.mal_d_sig_basename,
                                                args.mal_n_sig_basename)
  task_end_time = time.time()
  print
  print("Found %s possible distributed counters." % str(len(dist_counters)))
  if sws.DEBUG_PRINTS and dist_counters:
    for dist_counter in dist_counters:
      print("\tDistributed Counter: %s (Size: %d)" %
            (dist_counter.fullname(), dist_counter.width))
      dist_counter.debug_print()
  dist_counter_sizes = get_counter_sizes(dist_counters)
  print
  calculate_and_print_time(task_start_time, task_end_time)

  # Write counter sizes to JSON file
  export_sizes_json(coal_counter_sizes, dist_counter_sizes,
                    args.json_base_filename + ".sizes.json")

  # Classify counter behaviors
  analyze_counters(signals, vcd, coal_counters, dist_counters, args.start_time,
                   args.time_limit, args.time_resolution,
                   args.json_base_filename)

  # Stop analysis timer
  overall_end_time = time.time()
  calculate_and_print_time(overall_start_time, overall_end_time)


if __name__ == "__main__":
  main()
