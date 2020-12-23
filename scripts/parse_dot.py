import re
import sys

from connection import Connection
from hdl_signal import HDL_Signal

# Node Shapes
SIGNAL_NODE_SHAPE = "ellipse"
LOCAL_SIGNAL_NODE_SHAPE = "none"
FF_NODE_SHAPE = "square"
INPUT_NODE_SHAPE = "rectangle"
CONST_NODE_SHAPE = "none"

# Constant Prefix
CONST_HIERARCHY = 'const.'


def parse_file(file_name):
  num_signals = 0
  num_connections = 0

  # Open the dot file and read in DFG
  with open(file_name, 'r') as f:
    read_contents = f.read()
  lines = read_contents.splitlines()

  if "digraph G {" not in lines[0]:
    sys.stderr.write("Error: Invalid file format\n")
    sys.exit(1)

  if len(lines) >= (1 << 20):
    sys.stderr.write("Error: Over a million signals. Too many to handle\n")
    sys.exit(1)

  # Create signals dictionary
  signals = {}

  # Compile Regexes
  signals_re = re.compile(
      "\"[\w\.]+(\$[\d]+\$)?\" \[shape=([A-Za-z]+), label=\"([\w\.]+)(\$([\d]+)\$)?\[(\d+):(\d+)\]\"\]"
  )
  conns_re = re.compile(
      "\"([\w\.]+)(\$([\d]+)\$)?\" -> \"([\w\.]+)(\$([\d]+)\$)?\"\[label=\"\[(\d+):(\d+)\]->\[(\d+):(\d+)\]\"\]"
  )

  # Parse all signals first
  lineno = 1
  print("Parsing signals...")
  while "}" not in lines[lineno] and lineno < len(lines):

    if "->" not in lines[lineno]:

      # Parse signal information
      m = signals_re.search(lines[lineno])

      # Check if signal information was found
      if m is None:
        print("Error: Bad (node) format at line " + str(lineno + 1))
        sys.exit(2)

      # Get signal info
      fullname = m.group(3)
      msb = int(m.group(6))
      lsb = int(m.group(7))
      shape = m.group(2)
      if m.group(5):
        array_ind = int(m.group(5))
      else:
        array_ind = None

      # Check signal is NOT a const
      if not fullname.startswith(CONST_HIERARCHY):

        # Create signal object
        s = HDL_Signal(fullname, msb, lsb, array_ind)

        # Mark if signal is an INPUT or FLIPFLOP
        if shape == INPUT_NODE_SHAPE:
          s.isinput = True
        elif shape == FF_NODE_SHAPE:
          s.isff = True

        # Add signal to dictionary
        num_signals += 1
        signals[s.fullname()] = s

    lineno += 1
  print("%d signals found." % (num_signals))

  # Parse all connections
  lineno = 1
  print
  print("Parsing connections...")

  while "}" not in lines[lineno] and lineno < len(lines):

    if "->" in lines[lineno]:

      # Parse connection information
      m = conns_re.search(lines[lineno])

      # Check if conection information was found
      if m is None:
        print("Error: Bad (connection) format at line " + str(lineno + 1))
        sys.exit(2)

      # Check source signal is NOT a const
      if not m.group(1).startswith(CONST_HIERARCHY):

        # Get connection info (sink signal)
        sink_msb = int(m.group(9))
        sink_lsb = int(m.group(10))
        if m.group(6):
          sink_array_ind = int(m.group(6))
          sink_sig = signals[m.group(4) + "[" + str(sink_array_ind) + "]"]
        else:
          sink_array_ind = None
          sink_sig = signals[m.group(4)]

        # Get connection info (source signal)
        source_msb = int(m.group(7))
        source_lsb = int(m.group(8))
        if m.group(3):
          source_array_ind = int(m.group(3))
          source_sig = signals[m.group(1) + "[" + str(source_array_ind) + "]"]
        else:
          source_array_ind = None
          source_sig = signals[m.group(1)]

        # Add connection information
        num_connections += 1
        c = Connection(sink_sig, sink_msb, sink_lsb, source_sig, source_msb,
                       source_lsb)
        signals[sink_sig.fullname()].add_conn(c)

    lineno += 1

  print("%d connections found.\n" % (num_connections))
  assert "}" in lines[lineno]
  assert lineno + 1 == len(lines)
  return signals
