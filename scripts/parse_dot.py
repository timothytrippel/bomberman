import re
import sys

##
# Signal
##
# Holds all the info for a signal
class Signal:
	def __init__(self, name, msb, lsb):
		self.name        = name
		self.local_name  = name.split('.')[-1]
		self.lsb         = lsb
		self.msb         = msb
		self.isff        = False
		self.isinput     = False
		self.conn        = [] 
		self.tb_covered  = False
		self.hierarchy   = None
		self.width       = None
		self.type        = None
		self.time_values = {}

	def __str__(self):
		return self.name + "[" + str(self.msb) + ":" + str(self.lsb) + "]"

	def fullname(self):
		return self.name

	def connections(self):
		return self.conn;

	def add_conn(self, c):
		self.conn.append(c)

	def add_vcd_sim(self, vcd_data):
		# Load VCD Signal Info
		self.tb_covered = True
		self.hierarchy  = vcd_data['nets'][0]['hier']
		self.width      = int(vcd_data['nets'][0]['size'])
		self.type       = vcd_data['nets'][0]['type']

		# Load Simulation Time Values
		for tv in vcd_data['tv']:
			time   = tv[0]
			values = tv[1]
			assert time not in self.time_values.keys()
			self.time_values[time] = values

		# Check names and widths of match Dot file
		assert (self.hierarchy + '.' + self.local_name) == self.name
		assert self.width == (self.msb + 1 - self.lsb)

	def debug_print(self):
		print "	Signal: %s"            % (self)
		print "		Full Name:     %s" % (self.name)
		print "		Local Name:    %s" % (self.local_name)
		print "		Is Flip-Flop:  %s" % (self.isff)
		print "		Is Input:      %s" % (self.isinput)
		print "		Is TB Covered: %s" % (self.tb_covered)
		if self.tb_covered:
			print "		Is TB Covered: %s"   % (self.tb_covered)
			print "		Hierarchy:     %s"   % (self.hierarchy)
			print "		Width:         %d"   % (self.width)
			print "		Type:          %s"   % (self.type)
			print "		Connections   (%d):" % (len(self.conn))
			for connection in self.conn:
				print "			%s" % (connection)
			print "		Time Values.  (%d):" % (len(self.time_values.keys()))
			for time in self.time_values.keys():
				values = self.time_values[time]
				print "			%d -- %s" % (time, values)

##
# Connection
##
# Holds two signals which are connected. Also, since this connection may
# occur on slices of the signal, I store additional msb and lsb values
class Connection:
	def __init__(self, dep_sig, d_msb, d_lsb, sig, msb, lsb):
		self.dep_sig = dep_sig
		self.d_msb = d_msb
		self.d_lsb = d_lsb
		self.sig = sig
		self.msb = msb
		self.lsb = lsb

	def __str__(self):
		out = self.dep_sig.fullname() + "[" + str(self.d_msb) + ":" + str(self.d_lsb) + "]"
		out += " <-- "
		out += self.sig.fullname() + "[" + str(self.msb) + ":" + str(self.lsb) + "]"
		return out

	# def debug_print(self):


# Builds out the dependencies for each signal passed in
# @TODO: Currently inefficient, re-computing a lot of things
def build_deps(sig, msb, lsb, ffs = [], seen = {}):
	# print "Building Dependencies..."
	# print "Sig:",  sig
	# print "MSB:",  msb
	# print "LSB",   lsb
	# print "FFs:",  ffs
	# print "Seen:", seen
	# print "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
	# sig.debug_print()

	# If we've seen this exact slice before, continue
	if str(sig) + str(msb) + str(lsb) in seen.keys():
		# print "SEEN"
		return ffs

	# Mark this signal as seen
	seen[str(sig) + str(msb) + str(lsb)] = True

	# If its a FF or input, it is the end
	if (sig.isff or sig.isinput):
		# print "END"
		# Make sure we aren't already inserted
		for ff in ffs:
			if str(ff) == str(sig):
				return ffs

		ffs.append(Signal(sig.fullname(), msb, lsb))
		return ffs

	for c in sig.conn:
		build_deps(c.sig, c.msb, c.lsb, ffs, seen)

	return ffs

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
		s = Signal(m.group(2), int(m.group(3)), int(m.group(4)))

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

def generate_distributed_counters(signals):
	deps = []
	seen = {}

	for sig_name, sig in signals.iteritems():
		# print "Root Signal:"
		# sig.debug_print()

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

		# Stringfy list of flip-flop signal names
		distr = ','.join([str(ff) for ff in ffs])
		if distr not in seen:
			seen[distr] = True
			deps.append(ffs)

	return deps
