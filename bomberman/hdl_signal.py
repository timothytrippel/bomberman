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

import sys


class HDL_Signal:
  """Holds all the info for an HDL signal."""
  def __init__(self, name, msb, lsb, array_ind):
    self.local_name = name.split('.')[-1]
    self.hierarchy = name[0:-len(self.local_name + '.')]
    self.lsb = lsb
    self.msb = msb
    self.array_ind = array_ind
    self.isff = False
    self.isinput = False
    self.conn = []
    self.vcd_symbol = None
    self.tv_index = 0
    self.visited = False

  def width(self):
    return (self.msb - self.lsb + 1)

  def is_simulated(self):
    if self.vcd_symbol:
      return True
    else:
      return False

  def basename(self):
    return (self.hierarchy + '.' + self.local_name)

  def fullname(self):
    if self.array_ind is not None:
      return (self.basename() + '[' + str(self.array_ind) + ']')
    else:
      return self.basename()

  def sliced_fullname(self):
    return self.fullname() + "[" + str(self.msb) + ":" + str(self.lsb) + "]"

  def connections(self):
    return self.conn

  def get_sorted_time_values(self, vcd):

    # Check signal was simulated
    assert self.is_simulated()

    # Get all time values
    tvs = vcd[self.vcd_symbol]['tv']

    # Sort time values
    return sorted(tvs, key=lambda x: x[0])

  def get_time_value_at_index(self, vcd, index):

    # Check signal was simulated
    assert self.is_simulated()

    # Get time value at index
    tv = vcd[self.vcd_symbol]['tv'][index]

    return tv[0], tv[1]

  def get_value_at_time(self, vcd, time):

    # Check signal was simulated
    assert self.is_simulated()

    # Iterate over time values
    curr_time = None
    curr_value = None
    for i in range(len(vcd[self.vcd_symbol]['tv'])):
      curr_time = vcd[self.vcd_symbol]['tv'][i][0]
      curr_value = vcd[self.vcd_symbol]['tv'][i][1]
      if curr_time == time:
        return curr_value
      elif curr_time > time:
        if i > 0:
          return vcd[self.vcd_symbol]['tv'][i - 1][1]
        else:
          print("ERROR: no value for time (%d)" % time)
          sys.exit(-1)

    if curr_value:
      return curr_value
    else:
      print("ERROR: no value for time (%d)" % time)
      sys.exit(-1)

  def get_time_value(self, vcd, time_limit):

    # Check signal was simulated
    assert self.is_simulated()

    # Get all time values
    tvs = vcd[self.vcd_symbol]['tv']

    # Check if reached last index of time values
    if (self.tv_index == len(tvs)):
      return None, None

    # Get current time value
    current_tv = tvs[self.tv_index]
    current_time = current_tv[0]
    current_value = current_tv[1]

    # Increment time value index and return value
    if current_time <= time_limit:
      self.tv_index += 1
      return current_time, current_value
    else:
      return None, None

  def add_conn(self, c):
    self.conn.append(c)

  def debug_print(self):
    print("\tSignal: %s" % (self.fullname()))
    print("\t\tHierarchy:    %s" % (self.hierarchy))
    print("\t\tLocal Name:   %s" % (self.local_name))
    print("\t\tLSB:          %d" % (self.lsb))
    print("\t\tMSB:          %d" % (self.msb))
    print("\t\tWidth:        %d" % (self.width()))
    print("\t\tIs Flip-Flop: %s" % (self.isff))
    print("\t\tIs Input:     %s" % (self.isinput))
    print("\t\tVCD Symbol:   %s" % (self.vcd_symbol))
    print("\t\tTV Index:     %d" % (self.tv_index))

  def debug_print_wtvs(self, vcd):
    self.debug_print()
    if self.vcd_symbol:
      tvs = vcd[self.vcd_symbol]['tv']
      print("		Time Values    (%d):" % len(tvs))
      for tv in tvs:
        print("			%4d -- %s" % (tv[0], tv[1]))
