from malicious_counter import add_malicious_coal_counters


def generate_coalesced_counters(signals, vcd, num_mal_cntrs, dut_top_module,
                                d_sig_basename, n_sig_basename):
  coal_counters = {}

  # Find existing coalesced counters in the design
  # (i.e., signals that reside in the DUT and are flip-flops)
  for signal_name in signals.keys():
    if signals[signal_name].isff and signal_name.startswith(dut_top_module):
      coal_counters[signal_name] = signals[signal_name]

  # Generate artificial coalesced counters
  if num_mal_cntrs > 0:
    print("Generating Malicious Coalesced Counters...")
    coal_counters = add_malicious_coal_counters(signals, vcd, coal_counters,
                                                num_mal_cntrs, dut_top_module,
                                                d_sig_basename, n_sig_basename)
  return coal_counters
