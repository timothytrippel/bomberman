# Standard Modules
import sys

# Custom Modules
from hdl_signal import HDL_Signal

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

	def get_event_signal_time_value_str(self, signals, time):
		tv_str = self.event_signal.get_time_value(signals, time)
		tv_str = tv_str[::-1][self.event_signal_bit : self.event_signal_bit + 1][::-1]
		return tv_str

	def get_event_signal_time_value_int(self, signals, time):
		return int(self.get_event_signal_time_value_str(signals, time), 2)

	def get_increment(self, signals, time):

		# Check if increment value is an integer or a signal name (str)
		if isinstance(self.increment, int):

			# is DERTERMINISTIC (i.e. an integer)
			return self.increment
		elif isinstance(self.increment, str):

			# is NON-DETERMINISTIC --> get value of increment signal at time
			tv_str = signals[self.increment].get_time_value(signals, time)
			tv_str = tv_str[::-1][self.increment_lsb : self.increment_msb + 1][::-1]
			return int(tv_str, 2)

		else:
			print "ERROR: unsupported count increment type:", type(increment)
			sys.exit(1)

def generate_malicious_time_values(signals, mal_counter):

	# Initialize counter at time 0
	counts = [mal_counter.initial_count]
	count         = mal_counter.initial_count
	format_string = "{0:0%sb}" % (mal_counter.width)
	mal_counter.hdl_signal.set_time_value(0, format_string.format(count))

	# Increment counter values based on event signal toggle times/values
	update_times  = mal_counter.event_signal.get_sorted_update_times(signals)
	for i in range(1, min(len(update_times), 2**mal_counter.width - 1)):

		# Get times
		before_time = update_times[i-1]
		curr_time   = update_times[i]

		# Get Values 
		if 'x' not in mal_counter.get_event_signal_time_value_str(signals, before_time):
			before_value = mal_counter.get_event_signal_time_value_int(signals, before_time)
		else:
			before_value = -1
		if 'x' not in mal_counter.get_event_signal_time_value_str(signals, curr_time):
			curr_value = mal_counter.get_event_signal_time_value_int(signals, curr_time)
		else:
			curr_value = -1
		
		# Only trigger even on RISING edge
		if before_value == 0 and curr_value == 1:

			# get counter increment
			counter_increment = mal_counter.get_increment(signals, curr_time)

			# increment the counter if the increment is non-zero
			if counter_increment:
				count += counter_increment

				# update the counter
				count_bitstring = format_string.format(count)

				# check we haven't exceeded the size of the counter
				assert len(count_bitstring) <= mal_counter.width and \
					"ERROR: malicious counter value exceeds maximum size." 

				# record the counter time value (bitstring)
				mal_counter.hdl_signal.set_time_value(curr_time, count_bitstring)

	return mal_counter

def generate_malicious_counter(signals, mal_counter):
	
	# Create (Malicious) Counter HDL Signal
	mal_counter.hdl_signal = HDL_Signal(mal_counter.fullname, mal_counter.width - 1, 0)

	# Initialize Counter Characteristics
	mal_counter.hdl_signal.is_ff         = True
	mal_counter.hdl_signal.is_input      = False
	mal_counter.hdl_signal.tb_covered    = True
	mal_counter.hdl_signal.hierarchy     = None
	mal_counter.hdl_signal.ref_hierarchy = None
	mal_counter.hdl_signal.type          = 'reg'

	# Get counter increment event signal from signal name
	event_signal = None
	for signal_name in signals.keys():
		if signal_name == mal_counter.event_signal_fullname:
			if event_signal == None:
				mal_counter.event_signal = signals[signal_name]
				# print "Found event signal:", mal_counter.event_signal_fullname
				# mal_counter.event_signal.debug_print(signals)
				# print
			else:
				print "ERROR: non-unique event signal for adding malicious counter." 
				sys.exit(1)
	
	# Sanity checks
	assert mal_counter.event_signal and \
		"ERROR: cannot find counter increment event signal."
	assert mal_counter.event_signal.width > mal_counter.event_signal_bit and \
		"ERROR: event signal bit index outside event signal vector bounds."

	# Generate malicious counter time values
	mal_counter = generate_malicious_time_values(signals, mal_counter)
	
	# mal_counter.hdl_signal.debug_print(signals)

	return mal_counter.hdl_signal

# Add Malicious Coalesced Counters
def add_malicious_coal_counters(signals, coal_counters):

	# CDD
	# print "Generating Malicious (CDD) Counter:"
	cdd = MaliciousCounter()
	cdd.fullname              = 'ttb_test_aes_128.dut.mal_cdd'
	cdd.width                 = 8
	cdd.event_signal_fullname = 'ttb_test_aes_128.dut.clk'
	cdd.event_signal_bit      = 0
	cdd.initial_count         = 0
	cdd.increment             = 1
	cdd.increment_msb         = 0
	cdd.increment_lsb         = 0
	coal_counters.append(generate_malicious_counter(signals, cdd))

	# CDN
	# print "Generating Malicious (CDN) Counter:"
	cdn = MaliciousCounter()
	cdn.fullname              = 'ttb_test_aes_128.dut.mal_cdn'
	cdn.width                 = 8
	cdn.event_signal_fullname = 'ttb_test_aes_128.dut.key'
	cdn.event_signal_bit      = 44
	cdn.initial_count         = 0
	cdn.increment             = 1
	cdn.increment_msb         = 0
	cdn.increment_lsb         = 0
	coal_counters.append(generate_malicious_counter(signals, cdn))

	# CND
	# print "Generating Malicious (CND) Counter:"
	cnd = MaliciousCounter()
	cnd.fullname              = 'ttb_test_aes_128.dut.mal_cnd'
	cnd.width                 = 8
	cnd.event_signal_fullname = 'ttb_test_aes_128.dut.clk'
	cnd.event_signal_bit      = 0
	cnd.initial_count         = 0
	cnd.increment             = 'ttb_test_aes_128.dut.key'
	cnd.increment_msb         = 2
	cnd.increment_lsb         = 1
	coal_counters.append(generate_malicious_counter(signals, cnd))

	# CNN
	# print "Generating Malicious (CNN) Counter:"
	cnn = MaliciousCounter()
	cnn.fullname              = 'ttb_test_aes_128.dut.mal_cnn'
	cnn.width                 = 8
	cnn.event_signal_fullname = 'ttb_test_aes_128.dut.key'
	cnn.event_signal_bit      = 44
	cnn.initial_count         = 0
	cnn.increment             = 'ttb_test_aes_128.dut.key'
	cnn.increment_msb         = 2
	cnn.increment_lsb         = 1
	coal_counters.append(generate_malicious_counter(signals, cnn))

	return coal_counters