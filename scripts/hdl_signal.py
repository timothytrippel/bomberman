##
# HDL_Signal
##
# Holds all the info for an HDL signal
class HDL_Signal:
	def __init__(self, name, msb, lsb):
		self.name          = name
		self.local_name    = name.split('.')[-1]
		self.lsb           = lsb
		self.msb           = msb
		self.isff          = False
		self.isinput       = False
		self.conn          = []
		self.tb_covered    = False
		self.hierarchy     = None
		self.ref_hierarchy = None
		self.width         = (self.msb + 1 - self.lsb)
		self.type          = None
		self.time_values   = {}

	def __str__(self):
		return self.name + "[" + str(self.msb) + ":" + str(self.lsb) + "]"

	def fullname(self):
		return self.name

	def ref_fullname(self):
		return (self.ref_hierarchy + '.' + self.local_name)

	def connections(self):
		return self.conn;

	def get_update_times(self, all_signals):
		if self.hierarchy == self.ref_hierarchy:
			return self.time_values.keys()
		else:
			return all_signals[self.ref_fullname()].time_values.keys()

	def get_sorted_update_times(self, all_signals):
		if self.hierarchy == self.ref_hierarchy:
			return sorted(self.time_values.keys(), key=lambda x: int(x))
		else:
			return sorted(all_signals[self.ref_fullname()].time_values.keys(), key=lambda x: int(x))

	def get_time_values(self, all_signals):
		if self.hierarchy == self.ref_hierarchy:
			return self.time_values
		else:
			return all_signals[self.ref_fullname()].time_values

	def get_time_value(self, all_signals, time):
		if self.hierarchy == self.ref_hierarchy:
			return self.time_values[time]
		else:
			return all_signals[self.ref_fullname()].time_values[time]

	def set_time_value(self, time, value):
		assert self.hierarchy == None and self.hierarchy == self.ref_hierarchy \
		and "ERROR: cannot set time values for VCD signals"
		self.time_values[time] = value

	def append_time_value(self, time, value):
		assert self.hierarchy == None and self.hierarchy == self.ref_hierarchy \
		and "ERROR: cannot append time values for VCD signals"
		self.time_values[time] += value

	def add_conn(self, c):
		self.conn.append(c)

	def add_vcd_sim(self, vcd_data):

		# Find matching VCD net
		matching_vcd_net_found = False
		for net_dict in vcd_data['nets']:
			if (net_dict['hier'] + '.' + net_dict['name'].split('[')[0]) == self.name:
				matching_vcd_net_found = True
				break
		assert matching_vcd_net_found

		# Load VCD Signal Info
		self.tb_covered = True
		self.hierarchy  = net_dict['hier']
		self.type       = net_dict['type']

		# Load Simulation Time Values
		for tv in vcd_data['tv']:
			time  = tv[0]
			value = ''.join(tv[1])
			assert time not in self.time_values.keys()
			self.time_values[time] = value

		# Check names and widths of matching signal in Dot file
		assert self.lsb   == int(vcd_data['lsb'])
		assert self.msb   == int(vcd_data['msb'])
		assert self.width == int(net_dict['size'])

		# self.debug_print()

		# Check wire type and isff/isinput match
		assert (self.type == 'reg') or \
			   (self.type == 'wire' and not self.isff)

	# Check that this signal has been exercised by the test bench
	def check_signal_simulated(self):
		if not self.tb_covered:
			print "WARNING: " + self.fullname() + " not found in VCD file."
		return self.tb_covered

	def debug_print(self, all_signals):
		print "	Signal: %s"            % (self)
		print "		Full Name:     %s" % (self.name)
		print "		Local Name:    %s" % (self.local_name)
		print "		LSB:           %d" % (self.lsb)
		print "		MSB:           %d" % (self.msb)
		print "		Width:         %d" % (self.width)
		print "		Is Flip-Flop:  %s" % (self.isff)
		print "		Is Input:      %s" % (self.isinput)
		print "		Is TB Covered: %s" % (self.tb_covered)
		# print "		Connections   (%d):" % (len(self.conn))
		# for connection in self.conn:
		# 	print "			%s" % (connection)
		if self.tb_covered:
			print "		Hierarchy:     %s"   % (self.hierarchy)
			print "		Type:          %s"   % (self.type)
			print "		Time Values   (%d):" % (len(self.get_update_times(all_signals)))
			for time in self.get_sorted_update_times(all_signals):
				value = self.get_time_value(all_signals, time)
				print "			%4d -- %s" % (time, value)
