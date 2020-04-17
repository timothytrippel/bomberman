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
		self.tv_index       = 0
		self.visited        = False

	def width(self):
		return (self.msb - self.lsb + 1)

	def is_simulated(self):
		if self.vcd_symbol:
			return True
		else:
			return False

	def basename(self):
		return (self.hierarchy + '.' + self.local_name)

	def fullname(self):
		if self.array_ind != None:
			return (self.basename() + '[' + str(self.array_ind) + ']')
		else:
			return self.basename()

	def sliced_fullname(self):
		return self.fullname() + "[" + str(self.msb) + ":" + str(self.lsb) + "]"

	def connections(self):
		return self.conn

	def get_sorted_time_values(self, vcd):

		# Check signal was simulated
		assert self.is_simulated()

		# Get all time values
		tvs = vcd[self.vcd_symbol]['tv']

		# Sort time values
		return sorted(tvs, key=lambda x: x[0])

	def get_time_value_at_index(self, vcd, index):

		# Check signal was simulated
		assert self.is_simulated()

		# Get time value at index
		tv = vcd[self.vcd_symbol]['tv'][index]

		return tv[0], tv[1]

	def get_value_at_time(self, vcd, time):

		# Check signal was simulated
		assert self.is_simulated()

		# Iterate over time values
		curr_time  = None
		curr_value = None
		for i in range(len(vcd[self.vcd_symbol]['tv'])):
			curr_time  = vcd[self.vcd_symbol]['tv'][i][0]
			curr_value = vcd[self.vcd_symbol]['tv'][i][1]
			if curr_time == time:
				return curr_value
			elif curr_time > time:
				if i > 0:
					return vcd[self.vcd_symbol]['tv'][i - 1][1]
				else:
					print "ERROR: no value for time (%d)" % (time)
					sys.exit(-1)

		if curr_value:
			return curr_value
		else:
			print "ERROR: no value for time (%d)" % (time)
			sys.exit(-1)

	def get_time_value(self, vcd, time_limit):

		# Check signal was simulated
		assert self.is_simulated()

		# Get all time values
		tvs = vcd[self.vcd_symbol]['tv']

		# Check if reached last index of time values
		if (self.tv_index == len(tvs)):
			return None, None

		# Get current time value
		current_tv    = tvs[self.tv_index]
		current_time  = current_tv[0]
		current_value = current_tv[1]

		# Increment time value index and return value
		if current_time <= time_limit:	
			self.tv_index += 1
			return current_time, current_value
		else:
			return None, None

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
		print "		TV Index:       %d" % (self.tv_index)

	def debug_print_wtvs(self, vcd):
		self.debug_print()
		# print "		Connections   (%d):" % (len(self.conn))
		# for connection in self.conn:
		# 	print "			%s" % (connection)
		if self.vcd_symbol:
			tvs = vcd[self.vcd_symbol]['tv']
			print "		Time Values    (%d):" % (len(tvs))
			for tv in tvs:
				print "			%4d -- %s" % (tv[0], tv[1])
