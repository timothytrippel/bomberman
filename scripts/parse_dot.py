# Standard Modules
import re
import sys

# Custom Modules
from hdl_signal import *
from connection import *

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

# Builds out the dependencies for each signal passed in
# @TODO: Currently inefficient, re-computing a lot of things
def build_deps(sig, msb, lsb, ffs = [], seen = {}):
	# print "Building Dependencies..."
	# print "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
	# sig.debug_print()

	# If we've seen this exact slice before, continue
	if str(sig) + str(msb) + str(lsb) in seen.keys():
		return ffs

	# Mark this signal as seen
	seen[str(sig) + str(msb) + str(lsb)] = True

	# If its a FF or input, it is the end
	if (sig.isff or sig.isinput):
		# Make sure we aren't already inserted
		for ff in ffs:
			if str(ff) == str(sig):
				return ffs

		ffs.append(HDL_Signal(sig.fullname(), msb, lsb))
		return ffs

	for c in sig.conn:
		build_deps(c.sig, c.msb, c.lsb, ffs, seen)

	return ffs

def generate_distributed_counters(signals):
	deps = []
	seen = {}

	for sig_name, sig in signals.iteritems():
		# Compute Dependencies
		ffs = build_deps(sig, sig.msb, sig.lsb, [], {})
		
		# print "----------------------------------------------------------"
		# print "Dependencies:"
		# print "	%s:" % (sig)
		# for ff in ffs:
		# 	print "		%s" % (ff)
		# print "=========================================================="
		# print

		# No flip-flops found influencing signal
		if len(ffs) == 0:
			continue

		# Only 1 flip-flop found --> ignore if it is itself
		# @TODO-Tim --> check that ignore is only for self, not all signals.keys()
		if len(ffs) == 1:
			if ffs[0].fullname() in signals.keys():
				continue

		# Num flip-flops found is greater than 1 --> counter is distributed
		ffs.sort(key=str) # sort flip-flops alphabetically by name

		# # Create new HDL_Signal object for distributed counter
		# dist_counter = HDL_Signal("distributed", 0, 0)
		# dist_counter.lsb   = 0
		# dist_counter.msb   = 0
		# dist_counter.width = 0
		# for hdl_signal in ffs:
		# 	if 
		# 	dist_counter.ls

		# Stringfy list of flip-flop signal names
		distr = ','.join([str(ff) for ff in ffs])
		if distr not in seen:
			seen[distr] = True
			deps.append(ffs)

	return deps
