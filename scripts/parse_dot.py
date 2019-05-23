# Standard Modules
import re
import sys

# Custom Modules
from hdl_signal import HDL_Signal
from connection import Connection

# Node Shapes
SIGNAL_NODE_SHAPE       = "ellipse"
LOCAL_SIGNAL_NODE_SHAPE = "none"
FF_NODE_SHAPE           = "square"
INPUT_NODE_SHAPE        = "rectangle"
CONST_NODE_SHAPE        = "none"

def parse_file(file_name):
	
	# counters
	num_signals     = 0
	num_connections = 0

	# Open the dot file for reading
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

	# Parse all signals first
	print 
	lineno  =  1
	print "Parsing signals..."
	while "}" not in lines[lineno] and lineno < len(lines):

		if "->" not in lines[lineno]:

			# Parse signal information
			m = re.search('"[\w\.]+" \[shape=([A-Za-z]+), label="([\w\.]+)\[(\d+):(\d+)\]"\]', lines[lineno])
			
			# Check if signal information was found
			if (None == m):
				print "Error: Bad format at line " + str(lineno)
				sys.exit(2)

			# Get signal info
			name = m.group(2)
			msb  = int(m.group(3)),
			lsb  = int(m.group(4))
			
			# Create signal object
			s = HDL_Signal(m.group(2), int(m.group(3)), int(m.group(4)))

			# Mark if signal is an INPUT or FLIPFLOP
			if m.group(1) == INPUT_NODE_SHAPE:
				s.isinput = True
			elif m.group(1) == FF_NODE_SHAPE:
				s.isff = True

			# Add signal to dictionary
			num_signals += 1
			signals[s.fullname()] = s

		lineno += 1
	print "%d signals found." % (num_signals)

	# Parse all connections
	lineno  =  1
	print
	print "Parsing connections..."

	while "}" not in lines[lineno] and lineno < len(lines):
		
		if "->" in lines[lineno]:

			# Parse connection information
			m = re.search('"([\w\.]+)" -> "([\w\.]+)"\[label="\[(\d+):(\d+)\]->\[(\d+):(\d+)\]"\]', lines[lineno])

			# Check if conection information was found
			if None == m:
				sys.stderr.write("Error: Invalid file format\n")
				sys.exit(1)

			# Get connection info (sink signal)
			dep_sig = signals[m.group(2)]
			d_msb   = int(m.group(5))
			d_lsb   = int(m.group(6))

			# Get connection info (source signal)
			sig     = signals[m.group(1)]
			msb     = int(m.group(3))
			lsb     = int(m.group(4))

			# Add connection information
			num_connections += 1
			c = Connection(dep_sig, d_msb, d_lsb, sig, msb, lsb)
			signals[dep_sig.fullname()].add_conn(c)

		lineno += 1

	print "%d connections found." % (num_connections)
	print
	
	assert "}" in lines[lineno]
	assert lineno + 1 == len(lines)

	return signals
	