# Standard Modules
import sys

# Custom Modules
import switches as sws

##
# HDL_Signal
##
# Holds all the info for an HDL signal
class HDL_Signal:
	def __init__(self, name, msb, lsb, array_ind):
		self.local_name     = name.split('.')[-1]
		self.hierarchy      = name[0:-len(self.local_name + '.')]
		self.lsb            = lsb
		self.msb            = msb
		self.array_ind      = array_ind
		self.isff           = False
		self.isinput        = False
		self.conn           = []
		self.tb_covered     = False
		self.ref_local_name = None
		self.ref_hierarchy  = None
		self.width          = (self.msb + 1 - self.lsb)
		self.type           = None
		self.time_values    = {}

	def basename(self):
		return (self.hierarchy + '.' + self.local_name)

	def fullname(self):
		if self.array_ind != None:
			return (self.basename() + '[' + str(self.array_ind) + ']')
		else:
			return self.basename()

	def sliced_fullname(self):
		return self.fullname() + "[" + str(self.msb) + ":" + str(self.lsb) + "]"

	def ref_fullname(self):
		if self.ref_hierarchy and self.ref_local_name:
			if self.array_ind != None:
				return (self.ref_hierarchy + '.' + self.ref_local_name + '[' + str(self.array_ind) + ']')
			else:
				return (self.ref_hierarchy + '.' + self.ref_local_name)
		else:
			return None

	def connections(self):
		return self.conn

	def get_update_times(self, all_signals):
		if not self.tb_covered:
			self.debug_print()
			print self.time_values
		assert self.tb_covered
		return self.get_time_values(all_signals).keys()

	def get_sorted_update_times(self, all_signals):
		return sorted(self.get_update_times(all_signals), key=lambda x: int(x))

	def get_time_values(self, all_signals):
		if self.tb_covered and self.ref_hierarchy and self.ref_local_name:
			if self.fullname() == self.ref_fullname():
				return self.time_values
			else:
				return all_signals[self.ref_fullname()].time_values
		else:
			return self.time_values

	def get_time_value(self, all_signals, time):

		# Get all time values
		tvs = self.get_time_values(all_signals)

		# Indicator flag
		time_value_found = False
	
		# Check if value exists for a given time
		if time in tvs:
			return tvs[time]

		# If not --> return value from previous update time
		else:

			# Sort update times
			sorted_times = self.get_sorted_update_times(all_signals)
			
			# Iterate backwards over sorted times
			# until an earlier time is found
			i = len(sorted_times) - 1
			while i >= 0:

				# Get (earlier) time
				earlier_time = sorted_times[i]

				# Check if earlier time is less than the time requested
				if earlier_time < time:
					return tvs[earlier_time]

				# Decrement i
				i -= 1

			# If we reached here --> we have failed to find a time value
			print "ERROR: unkown time value for signal (%s) at time (%d)" %\
				(self.fullname(), time)
			sys.exit(1) 

	def set_time_value(self, time, value):
		assert self.ref_hierarchy == None and self.ref_local_name == None \
		and "ERROR: cannot set time values for VCD signals"
		self.time_values[time] = value

	def append_time_value(self, time, value):
		assert self.ref_hierarchy == None and self.ref_local_name == None \
		and "ERROR: cannot append time values for VCD signals"
		self.time_values[time] += value

	def add_conn(self, c):
		self.conn.append(c)

	def add_vcd_sim(self, vcd_data):

		# Only process regs and wires (i.e. synthesizable constructs)
		if self.type == 'reg' or self.type == 'wire':

			# Find matching VCD net
			matching_vcd_net_found = False
			for net_dict in vcd_data['nets']:
				if (self.hierarchy == net_dict['hier']):
					matching_vcd_net_found = True
					break
			assert matching_vcd_net_found

			# Load VCD Signal Info
			self.tb_covered = True
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

	def debug_print(self):
		print "	Signal: %s"             % (self.fullname())
		print "		Hierarchy:      %s" % (self.hierarchy)
		print "		Local Name:     %s" % (self.local_name)
		print "		LSB:            %d" % (self.lsb)
		print "		MSB:            %d" % (self.msb)
		print "		Width:          %d" % (self.width)
		print "		Is Flip-Flop:   %s" % (self.isff)
		print "		Is Input:       %s" % (self.isinput)
		print "		Is TB Covered:  %s" % (self.tb_covered)
		print "		Ref-Hierarchy:  %s" % (self.ref_hierarchy)
		print "		Ref-Local Name: %s" % (self.ref_local_name)


	def debug_print_wtvs(self, all_signals):
		print "	Signal: %s"            % (self)
		print "		Hierarchy:     %s" % (self.hierarchy)
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
			print "		Ref-Hierarchy:  %s"   % (self.ref_hierarchy)
			print "		Ref-Local Name: %s"   % (self.ref_local_name)
			print "		Type:           %s"   % (self.type)
			print "		Time Values    (%d):" % (len(self.get_update_times(all_signals)))
			for time in self.get_sorted_update_times(all_signals):
				value = self.get_time_value(all_signals, time)
				print "			%4d -- %s" % (time, value)
