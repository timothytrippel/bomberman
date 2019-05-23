# Standard Modules
import sys

# Custom Modules
from hdl_signal import HDL_Signal

# def gen_mal_coal_counter(inc_type, event_type, width, inc_signal, event_signal):
# 	# Create M

# 	return coal_counters

def generate_mal_time_values(signals, mal_counter, fullname, width, event_signal, es_msb, es_lsb, initial_count, count_inc):

	# Increment counter values based on event signal toggle times/values
	count         = initial_count
	format_string = "{0:0%sb}" % (width)
	update_times  = event_signal.get_sorted_update_times(signals)
	for i in range(1, min(len(update_times), 2**width - 1)):

		# Get times
		before_time = update_times[i-1]
		curr_time   = update_times[i]

		# print before_time, '-->', event_signal.get_time_value(signals, before_time), es_lsb, es_msb

		# Get Values 
		if 'x' not in event_signal.get_time_value(signals, before_time):
			binary_str_value = event_signal.get_time_value(signals, before_time)[::-1][es_lsb : es_msb + 1][::-1]
			# print binary_str_value
			before_value     = int(binary_str_value, 2)
		else:
			before_value = -1
		if 'x' not in event_signal.get_time_value(signals, curr_time):
			binary_str_value = event_signal.get_time_value(signals, curr_time)[::-1][es_lsb : es_msb + 1][::-1]
			# print binary_str_value
			curr_value       = int(binary_str_value, 2) 
		else:
			curr_value = -1
		
		# Only trigger even on RISING edge
		if before_value == 0 and curr_value == 1:
			# print time, '-->', event_signal.get_time_value(signals, time)		
			mal_counter.set_time_value(curr_time, format_string.format(count))
			count += count_inc

	return mal_counter

def generate_malicious_counter(signals, fullname, width, event_signal_name, es_msb, es_lsb, initial_count, count_inc):
	
	# Create (Malicious) Counter HDL Signal
	mal_counter = HDL_Signal(fullname, width - 1, 0)

	# Initialize Counter Characteristics
	mal_counter.is_ff         = True
	mal_counter.is_input      = False
	mal_counter.tb_covered    = True
	mal_counter.hierarchy     = None
	mal_counter.ref_hierarchy = None
	mal_counter.type          = 'reg'

	# Get counter increment event signal from signal name
	event_signal = None
	for signal_name in signals.keys():
		if signal_name == event_signal_name:
			if event_signal == None:
				event_signal = signals[signal_name]
				print "Found event signal:", event_signal_name
				# event_signal.debug_print(signals)
				print
			else:
				print "ERROR: non-unique event signal for adding malicious counter." 
				sys.exit(1)
	assert event_signal and "ERROR: cannot find counter increment event signal."

	# Generate Malicious Counter
	mal_counter = generate_mal_time_values(signals, mal_counter, fullname, width, event_signal, es_msb, es_lsb, initial_count, count_inc)
	mal_counter.debug_print(signals)

def generate_coalesced_counters(signals):
	coal_counters = []

	for signal_name in signals.keys():

		# Check if signal is a flip-flop
		if signals[signal_name].isff:
			coal_counters.append(signals[signal_name])

	# Add Malicious Counters
	
	# # CDD
	# print "Generating Malicious (CDD) Counter:"
	# generate_malicious_counter(signals, 'ttb_test_aes_128.dut.mal_cdd', 8, 'ttb_test_aes_128.dut.clk', 0, 0, 0, 1)

	# # CDN
	# print "Generating Malicious (CDN) Counter:"
	# generate_malicious_counter(signals, 'ttb_test_aes_128.dut.mal_cdn', 8, 'ttb_test_aes_128.dut.key', 44, 44, 0, 1)

	

	return coal_counters
