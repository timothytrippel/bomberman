# Standard Modules
import sys

# Custom Modules
import switches   as     sws
from   hdl_signal import HDL_Signal

class MaliciousCounter:
	def __init__(self):
		self.fullname              = None
		self.width                 = 1
		self.event_signal          = None
		self.event_signal_fullname = None
		self.event_signal_bit      = 0
		self.initial_count         = 0
		self.increment             = 1
		self.increment_msb         = 0
		self.increment_lsb         = 0
		self.hdl_signal            = None
		self.vcd_dict              = {'nets': {}, 'tv': []}

	def get_increment(self, signals, vcd, time):

		# Check if increment value is an integer or a signal name (str)
		if isinstance(self.increment, int):

			# is DERTERMINISTIC (i.e. an integer)
			return self.increment
		elif isinstance(self.increment, str):

			# is NON-DETERMINISTIC --> get value of increment signal at time
			value = signals[self.increment].get_value_at_time(vcd, time)
			value = value[::-1][self.increment_lsb : self.increment_msb + 1][::-1]
			return int(value, 2)

		else:
			print "ERROR: unsupported count increment type:", type(increment)
			sys.exit(1)

	def add_vcd_tv(self, time, value):
		self.vcd_dict['tv'].append((time, value))

def generate_malicious_time_values(signals, vcd, mal_counter):

	# Initialize counter at time 0
	counts        = [mal_counter.initial_count]
	count         = mal_counter.initial_count
	format_string = "{0:0%sb}" % (mal_counter.width)
	# print vcd.items()[0][1]['nets'][0]
	# print vcd.items()[0][1]['tv'][0]
	mal_counter.add_vcd_tv(0, format_string.format(count))

	# Increment counter values based on event signal toggle times/values
	sorted_tvs = mal_counter.event_signal.get_sorted_time_values(vcd)
	for i in range(1, min(len(sorted_tvs), 2**mal_counter.width - 1)):

		# Get times and values
		before_time  = sorted_tvs[i-1][0]
		before_value = sorted_tvs[i-1][1][::-1][mal_counter.event_signal_bit : mal_counter.event_signal_bit + 1][::-1]
		before_value = int(before_value)
		curr_time    = sorted_tvs[i][0]
		curr_value   = sorted_tvs[i][1][::-1][mal_counter.event_signal_bit : mal_counter.event_signal_bit + 1][::-1]
		curr_value   = int(curr_value)
		# print "Before  Time/Value:", before_time, "/", before_value, sorted_tvs[i-1]
		# print "Current Time/Value:", curr_time, "/", curr_value
		# print

		# Only trigger even on RISING edge
		if before_value == 0 and curr_value == 1:

			# get counter increment
			counter_increment = mal_counter.get_increment(signals, vcd, curr_time)

			# increment the counter if the increment is non-zero
			if counter_increment:
				count += counter_increment

				# update the counter
				count_bitstring = format_string.format(count)

				# check we haven't exceeded the size of the counter
				assert len(count_bitstring) <= mal_counter.width and \
					"ERROR: malicious counter value exceeds maximum size." 

				# record the counter time value (bitstring)
				mal_counter.add_vcd_tv(curr_time, count_bitstring)

	# Update VCD dictionary
	vcd[mal_counter.fullname] = mal_counter.vcd_dict

	return mal_counter, vcd

def generate_malicious_counter(signals, vcd, mal_counter):
	
	# Create (Malicious) Counter HDL Signal
	mal_counter.hdl_signal = HDL_Signal(mal_counter.fullname, mal_counter.width - 1, 0, None)

	# Initialize Counter VCD Data HDL Signal
	# VCD
	mal_counter.vcd_dict['nets']['type'] = 'reg'
	mal_counter.vcd_dict['nets']['name'] = mal_counter.fullname
	mal_counter.vcd_dict['nets']['size'] = mal_counter.width
	mal_counter.vcd_dict['nets']['hier'] = '.'.join(mal_counter.fullname.split('.')[0:-1])
	# HDL_Signal
	mal_counter.hdl_signal.is_ff      = True
	mal_counter.hdl_signal.is_input   = False
	mal_counter.hdl_signal.vcd_symbol = mal_counter.fullname

	# Get counter increment event signal from signal name
	event_signal = None
	for signal_name in signals.keys():
		if signal_name == mal_counter.event_signal_fullname:
			if event_signal == None:
				mal_counter.event_signal = signals[signal_name]
				# reset event signal index
				print "Found event signal:", mal_counter.event_signal_fullname
				mal_counter.event_signal.debug_print()
				print
			else:
				print "ERROR: non-unique event signal for adding malicious counter." 
				sys.exit(1)
	
	# Sanity checks
	assert mal_counter.event_signal and \
		"ERROR: cannot find counter increment event signal."
	assert mal_counter.event_signal.width > mal_counter.event_signal_bit and \
		"ERROR: event signal bit index outside event signal vector bounds."

	# Generate malicious counter time values
	mal_counter, vcd = generate_malicious_time_values(signals, vcd, mal_counter)
	
	if sws.VERBOSE > 1:
		mal_counter.hdl_signal.debug_print_wtvs(vcd)

	return mal_counter.hdl_signal, vcd

# Add Malicious Coalesced Counters
def add_malicious_coal_counters(signals, vcd, coal_counters, num_cntrs):

	# CDD
	if sws.VERBOSE:
		print "Generating Malicious (CDD) Counter..."
	cdd = MaliciousCounter()
	cdd.fullname                = 'ttb_test_aes_128.dut.mal_cdd'
	cdd.width                   = 8
	cdd.event_signal_fullname   = 'ttb_test_aes_128.dut.clk'
	cdd.event_signal_bit        = 0
	cdd.initial_count           = 0
	cdd.increment               = 1
	cdd.increment_msb           = 0
	cdd.increment_lsb           = 0
	coal_counters[cdd.fullname], vcd = generate_malicious_counter(signals, vcd, cdd)

	# CDN
	if sws.VERBOSE:
		print "Generating Malicious (CDN) Counter..."
	cdn = MaliciousCounter()
	cdn.fullname              = 'ttb_test_aes_128.dut.mal_cdn'
	cdn.width                 = 8
	cdn.event_signal_fullname = 'ttb_test_aes_128.dut.key'
	cdn.event_signal_bit      = 44
	cdn.initial_count         = 0
	cdn.increment             = 1
	cdn.increment_msb         = 0
	cdn.increment_lsb         = 0
	coal_counters[cdn.fullname], vcd = generate_malicious_counter(signals, vcd, cdn)

	# CND
	if sws.VERBOSE:
		print "Generating Malicious (CND) Counter..."
	cnd = MaliciousCounter()
	cnd.fullname              = 'ttb_test_aes_128.dut.mal_cnd'
	cnd.width                 = 8
	cnd.event_signal_fullname = 'ttb_test_aes_128.dut.clk'
	cnd.event_signal_bit      = 0
	cnd.initial_count         = 0
	cnd.increment             = 'ttb_test_aes_128.dut.key'
	cnd.increment_msb         = 2
	cnd.increment_lsb         = 1
	coal_counters[cnd.fullname], vcd = generate_malicious_counter(signals, vcd, cnd)

	# CNN
	if sws.VERBOSE:
		print "Generating Malicious (CNN) Counter..."
	cnn = MaliciousCounter()
	cnn.fullname              = 'ttb_test_aes_128.dut.mal_cnn'
	cnn.width                 = 8
	cnn.event_signal_fullname = 'ttb_test_aes_128.dut.key'
	cnn.event_signal_bit      = 44
	cnn.initial_count         = 0
	cnn.increment             = 'ttb_test_aes_128.dut.key'
	cnn.increment_msb         = 2
	cnn.increment_lsb         = 1
	coal_counters[cnn.fullname], vcd = generate_malicious_counter(signals, vcd, cnn)

	return coal_counters

# Add Malicious Distributed Counters
def add_malicious_dist_counters(signals, vcd, dist_counters, num_cntrs):
	
	# DDD
	if sws.VERBOSE:
		print "Generating Malicious (DDD) Counter..."
	ddd = MaliciousCounter()
	ddd.fullname              = 'ttb_test_aes_128.dut.mal_ddd'
	ddd.width                 = 16
	ddd.event_signal_fullname = 'ttb_test_aes_128.dut.clk'
	ddd.event_signal_bit      = 0
	ddd.initial_count         = 0
	ddd.increment             = 1
	ddd.increment_msb         = 0
	ddd.increment_lsb         = 0
	dist_counters[ddd.fullname], vcd = generate_malicious_counter(signals, vcd, ddd)

	# DDN
	if sws.VERBOSE:
		print "Generating Malicious (DDN) Counter..."
	ddn = MaliciousCounter()
	ddn.fullname              = 'ttb_test_aes_128.dut.mal_ddn'
	ddn.width                 = 16
	ddn.event_signal_fullname = 'ttb_test_aes_128.dut.state'
	ddn.event_signal_bit      = 5
	ddn.initial_count         = 0
	ddn.increment             = 1
	ddn.increment_msb         = 0
	ddn.increment_lsb         = 0
	dist_counters[ddn.fullname], vcd = generate_malicious_counter(signals, vcd, ddn)

	# DND
	if sws.VERBOSE:
		print "Generating Malicious (DND) Counter..."
	dnd = MaliciousCounter()
	dnd.fullname              = 'ttb_test_aes_128.dut.mal_dnd'
	dnd.width                 = 16
	dnd.event_signal_fullname = 'ttb_test_aes_128.dut.clk'
	dnd.event_signal_bit      = 0
	dnd.initial_count         = 0
	dnd.increment             = 'ttb_test_aes_128.dut.state'
	dnd.increment_msb         = 3
	dnd.increment_lsb         = 0
	dist_counters[dnd.fullname], vcd = generate_malicious_counter(signals, vcd, dnd)

	# DNN
	if sws.VERBOSE:
		print "Generating Malicious (DNN) Counter..."
	dnn = MaliciousCounter()
	dnn.fullname              = 'ttb_test_aes_128.dut.mal_dnn'
	dnn.width                 = 16
	dnn.event_signal_fullname = 'ttb_test_aes_128.dut.state'
	dnn.event_signal_bit      = 5
	dnn.initial_count         = 0
	dnn.increment             = 'ttb_test_aes_128.dut.state'
	dnn.increment_msb         = 63
	dnn.increment_lsb         = 60
	dist_counters[dnn.fullname], vcd = generate_malicious_counter(signals, vcd, dnn)

	return dist_counters