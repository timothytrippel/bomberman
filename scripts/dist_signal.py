import sys


# Holds all the info for an sliced signal
class SignalSlice:
  def __init__(self, signal, msb, lsb):
    self.signal = signal
    self.msb = msb
    self.lsb = lsb
    self.tv_index = 0

  def width(self):
    return (self.msb - self.lsb + 1)

  def get_time_value_at_index(self, vcd, index):
    time, unsliced_value = self.signal.get_time_value_at_index(vcd, index)
    value = unsliced_value[self.signal.width() - self.msb -
                           1:self.signal.width() - self.lsb]
    return time, value


# Holds all the info for an Distributed signal
class DistSignal:
  def __init__(self, name, msb, is_simd, slices):
    self.name = name
    self.msb = msb
    self.signal_slices = slices
    self.enum_all_vals = False
    self.is_simd = is_simd

  def width(self):
    return (self.msb + 1)

  def fullname(self):
    return self.name

  def is_simulated(self):
    return self.is_simd

  def get_time_value(self, vcd, time_limit):
    current_time = 0
    current_value = ''
    next_time = -1

    if self.enum_all_vals:
      return None, None

    # Get time and value at current time-value index
    for sig_slice in self.signal_slices:
      time, value = sig_slice.get_time_value_at_index(vcd, sig_slice.tv_index)
      current_value += value
      if time > current_time:
        current_time = time

      # Check if another time-value exists
      if (sig_slice.tv_index + 1) < len(
          vcd[sig_slice.signal.vcd_symbol]['tv']):
        time, __ = sig_slice.get_time_value_at_index(vcd,
                                                     sig_slice.tv_index + 1)
        if time < next_time or next_time == -1:
          next_time = time

    # Check if over time limit constraint
    if current_time > time_limit:
      return None, None

    # Check if we've enumerated all values
    if next_time != -1:
      for sig_slice in self.signal_slices:
        if (sig_slice.tv_index + 1) < len(
            vcd[sig_slice.signal.vcd_symbol]['tv']):
          time, __ = sig_slice.get_time_value_at_index(vcd,
                                                       sig_slice.tv_index + 1)
          if time == next_time:
            sig_slice.tv_index += 1

    else:
      self.enum_all_vals = True

    # Return current time and value
    return current_time, current_value
