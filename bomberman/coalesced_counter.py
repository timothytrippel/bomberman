# Copyright © 2019, Massachusetts Institute of Technology
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

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
