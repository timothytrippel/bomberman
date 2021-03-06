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

import copy
import json


def compute_max_fanin(signals, dut_top_module, filename):
  total_inputs = 0
  total_sinks = 0
  max_fanin = 0
  local_fanins = []

  for sig in signals:
    num_inputs = len(signals[sig].conn)

    if dut_top_module in signals[sig].hierarchy and \
            not signals[sig].isinput and \
            num_inputs > 0:

      local_fanins.append(num_inputs)
      max_fanin = max(max_fanin, num_inputs)
      total_inputs += num_inputs
      total_sinks += 1

  print("Total Inputs   =", total_inputs)
  print("Total Sinks    =", total_sinks)
  print("Average Fan-in =", (float(total_inputs) / float(total_sinks)))
  print("Max Fan-in     =", max_fanin)
  json_dict = {"Fan-in": local_fanins}
  with open(filename, 'w') as jf:
    json.dump(json_dict, jf)
  jf.close()


def compute_max_bitwise_fanin(signals, dut_top_module, filename):
  total_num_input_bits = 0
  max_fanin = 0
  total_num_sink_bits = 0
  local_fanins = []

  for sig in signals:
    num_inputs = len(signals[sig].conn)
    sink_signal_width = signals[sig].width()
    num_input_bits = 0

    if (dut_top_module in signals[sig].hierarchy) and (
        not signals[sig].isinput) and (num_inputs > 0):
      for c in signals[sig].conn:
        num_input_bits += (c.source_msb - c.source_lsb + 1)
      local_fanin = float(num_input_bits) / float(sink_signal_width)
      local_fanins.append(local_fanin)
      max_fanin = max(max_fanin, local_fanin)
      total_num_input_bits += num_input_bits
      total_num_sink_bits += sink_signal_width

  print("Total Num Input Bits =", total_num_input_bits)
  print("Total Num Sink Bits  =", total_num_sink_bits)
  print("Average Fan-in       =",
        (float(total_num_input_bits) / float(total_num_sink_bits)))
  print("Max Fan-in     =", max_fanin)
  json_dict = {"Fan-in": local_fanins}
  with open(filename, 'w') as jf:
    json.dump(json_dict, jf)
  jf.close()


def compute_max_reg2reg_path(signals, dut_top_module, filename):
  # DFS on any signal to find average reg2reg path
  lengths = []

  def dfs(curr_sig, length):
    if curr_sig.isinput or curr_sig.isff:
      lengths.append(copy.deepcopy(length))
      return
    for conn in curr_sig.conn:
      if not conn.source_sig.visited:
        conn.source_sig.visited = True
        dfs(conn.source_sig, length + 1)
    return

  # find inputs/regs
  for sig in signals:
    if dut_top_module in signals[sig].hierarchy and \
            (signals[sig].isinput or signals[sig].isff):

      signals[sig].visited = True
      for conn in signals[sig].conn:
        if not conn.source_sig.visited:
          dfs(conn.source_sig, 1)

  for sig in signals:
    signals[sig].visited = False
  print("Total Path Length           =", sum(lengths))
  print("Total Paths                 =", len(lengths))
  if len(lengths) > 0:
    print("Average Reg2Reg Path Length =",
          (float(sum(lengths)) / float(len(lengths))))
    print("Max Reg2Reg Path Length     =", max(lengths))
  else:
    print("Average Reg2Reg Path Length =", 0.0)
    print("Max Reg2Reg Path Length     =", 0)
  json_dict = {"Reg2Reg Path Length": lengths}
  with open(filename, 'w') as jf:
    json.dump(json_dict, jf)
  jf.close()
