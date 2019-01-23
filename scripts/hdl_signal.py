##
# HDL_Signal
##
# Holds all the info for an HDL signal
class HDL_Signal:
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
		self.width       = (self.msb + 1 - self.lsb)
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
		self.type       = vcd_data['nets'][0]['type']

		# Load Simulation Time Values
		for tv in vcd_data['tv']:
			time   = tv[0]
			values = ''.join(tv[1])
			assert time not in self.time_values.keys()
			self.time_values[time] = values

		# Check names and widths of matching signal in Dot file
		assert (self.hierarchy + '.' + self.local_name) == self.name
		assert self.width == int(vcd_data['nets'][0]['size'])
		assert vcd_data['lsb'] == self.lsb

	# Check that this signal has been exercised by the test bench
	def check_signal_simulated(self):
		if not self.tb_covered:
			print "WARNING: " + self.fullname() + " not found in VCD file."
		return self.tb_covered

	def debug_print(self):
		print "	Signal: %s"            % (self)
		print "		Full Name:     %s" % (self.name)
		print "		Local Name:    %s" % (self.local_name)
		print "		LSB:           %d" % (self.lsb)
		print "		MSB:           %d" % (self.msb)
		print "		Width:         %d" % (self.width)
		print "		Is Flip-Flop:  %s" % (self.isff)
		print "		Is Input:      %s" % (self.isinput)
		print "		Is TB Covered: %s" % (self.tb_covered)
		print "		Connections   (%d):" % (len(self.conn))
		for connection in self.conn:
			print "			%s" % (connection)
		if self.tb_covered:
			print "		Is TB Covered: %s"   % (self.tb_covered)
			print "		Hierarchy:     %s"   % (self.hierarchy)
			print "		Type:          %s"   % (self.type)
			print "		Time Values.  (%d):" % (len(self.time_values.keys()))
			for time in self.time_values.keys():
				values = self.time_values[time]
				print "			%d -- %s" % (time, values)
