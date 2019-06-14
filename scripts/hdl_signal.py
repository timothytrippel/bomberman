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
		self.vcd_symbol     = None
		self.time_values    = []
		# self.tb_covered     = False
		# self.ref_local_name = None
		# self.ref_hierarchy  = None
		# self.width          = (self.msb + 1 - self.lsb)
		# self.type           = None
		# self.time_values    = {}

	def width(self):
		return (self.msb - self.lsb + 1)

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

	def get_time_values(self, vcd):
		if self.vcd_symbol:
			# print vcd[self.vcd_symbol]
			return vcd[self.vcd_symbol]['tv']
		else:
			return self.time_values

	def get_time_value(self, vcd, time):

		# Get all time values
		tvs = self.get_time_values(vcd)

		# Indicator flag
		time_value_found = False
	
		# Check if value exists for a given time
		update_times = zip(*tvs)[0]
		if time in update_times:
			return tvs[update_times.index(time)][1]

		# If not --> return value from previous update time
		else:
			
			# Iterate backwards over times (assuming they're sorted)
			# until an earlier time is found
			i = len(tvs) - 1
			while i >= 0:

				# Get (earlier) time
				earlier_time = tvs[i][0]

				# Check if earlier time is less than the time requested
				if earlier_time < time:
					return tvs[i][1]

				# Decrement i
				i -= 1

			# If we reached here --> we have failed to find a time value
			print "ERROR: unkown time value for signal (%s) at time (%d)" %\
				(self.fullname(), time)
			sys.exit(1) 

	def set_time_value(self, time, value):
		assert self.vcd_symbol == None \
		and "ERROR: cannot set time values for VCD signals"
		self.time_values.append([time, value])

	def append_time_value(self, time, value):
		assert self.vcd_symbol == None \
		and "ERROR: cannot append time values for VCD signals"
		for i in range(len(self.time_values) - 1, -1, -1):
			if time == self.time_values[i][0]:
				self.time_values[i][1] += value
				break

	def add_conn(self, c):
		self.conn.append(c)

	def debug_print(self):
		print "	Signal: %s"             % (self.fullname())
		print "		Hierarchy:      %s" % (self.hierarchy)
		print "		Local Name:     %s" % (self.local_name)
		print "		LSB:            %d" % (self.lsb)
		print "		MSB:            %d" % (self.msb)
		print "		Width:          %d" % (self.width())
		print "		Is Flip-Flop:   %s" % (self.isff)
		print "		Is Input:       %s" % (self.isinput)
		print "		VCD Symbol:     %s" % (self.vcd_symbol)

	def debug_print_wtvs(self, vcd):
		print "	Signal: %s"            % (self)
		print "		Hierarchy:     %s" % (self.hierarchy)
		print "		Local Name:    %s" % (self.local_name)
		print "		LSB:           %d" % (self.lsb)
		print "		MSB:           %d" % (self.msb)
		print "		Width:         %d" % (self.width())
		print "		Is Flip-Flop:  %s" % (self.isff)
		print "		Is Input:      %s" % (self.isinput)
		print "		VCD Symbol:    %s" % (self.vcd_symbol)
		# print "		Connections   (%d):" % (len(self.conn))
		# for connection in self.conn:
		# 	print "			%s" % (connection)
		if self.vcd_symbol:
			tvs = self.get_time_values(vcd)
			print "		Time Values    (%d):" % (len(tvs))
			for tv in tvs:
				print "			%4d -- %s" % (tv[0], tv[1])
