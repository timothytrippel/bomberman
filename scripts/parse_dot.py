# Standard Modules
import re
import sys

# Custom Modules
from hdl_signal import HDL_Signal
from connection import Connection

def parse_file(file_name):
	with open(file_name, 'r') as f:
		read_contents = f.read()

	lines = read_contents.splitlines()

	if "digraph G {" not in lines[0]:
		sys.stderr.write("Error: Invalid file format\n")
		sys.exit(1)

	lineno    =  1
	signals   = {}

	if len(lines) >= (1 << 20):
		sys.stderr.write("Error: Over a million signals. Too many to handle\n")
		sys.exit(1)

	while "->" not in lines[lineno] and lineno < len(lines):
		m = re.search('"[\w\.]+" \[shape=([A-Za-z]+), label="([\w\.]+)\[(\d+):(\d+)\]"\]', lines[lineno])
		if (None == m):
			print "Error: Bad format at line " + str(lineno)
			sys.exit(2)

		lineno += 1
		s = HDL_Signal(m.group(2), int(m.group(3)), int(m.group(4)))

		if m.group(1) == "none":
			s.isinput = True
		elif m.group(1) == "square":
			s.isff = True

		signals[s.fullname()] = s

	while "}" not in lines[lineno] and lineno < len(lines):
		m = re.search('"([\w\.]+)" -> "([\w\.]+)"\[label="\[(\d+):(\d+)\]->\[(\d+):(\d+)\]"\]', lines[lineno])

		if None == m:
			sys.stderr.write("Error: Invalid file format\n")
			sys.exit(1)

		dep_sig = signals[m.group(2)]
		d_msb   = int(m.group(5))
		d_lsb   = int(m.group(6))

		sig     = signals[m.group(1)]
		msb     = int(m.group(3))
		lsb     = int(m.group(4))

		c = Connection(dep_sig, d_msb, d_lsb, sig, msb, lsb)
		signals[dep_sig.fullname()].add_conn(c)

		lineno += 1

	assert "}" in lines[lineno]
	assert lineno + 1 == len(lines)

	return signals
	