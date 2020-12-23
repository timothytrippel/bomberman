#!/usr/bin/env python
import sys

from Verilog_VCD import parse_vcd


def extend_values(vcd):
  for symbol, signal in vcd.iteritems():
    for net in signal["nets"]:
      # If larger than one bit, zero extend or x extend
      if len(net["name"].split("[")) != 1:
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

  return vcd


##
# Parse vcd file
##
print "Parsing VCD File..."
vcd = parse_vcd(sys.argv[1])
print "Extending Values..."
vcd = extend_values(vcd)

for symbol, signal in vcd.iteritems():
  # Loop through values and count number of toggles
  for net in signal["nets"]:
    size = int(net['size'])
    last_value = ['x'] * size
    toggle_count = [0] * size
    for tvs in signal['tv']:
      new_value = tvs[1]
      for i in range(0, size):
        if new_value[i] != last_value[i]:
          toggle_count[i] += 1
          last_value[i] = new_value[i]
    for i in range(0, size):
      print net["hier"] + "." + net["name"] + "[" + str(size - i -
                                                        1) + "]" + " " + str(
                                                            toggle_count[i])
