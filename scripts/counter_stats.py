# Holds all the data for counters in a given design
class CounterStats:
  def __init__(self, counters, not_simd, consts, mal):
    self.num_total = len(counters)
    self.num_not_simd = len(not_simd)
    self.num_consts = len(consts)
    self.num_mal = len(mal)

  def print_stats(self):
    print("# Possible:  %d" % (self.num_total))
    print("# Not Simd:  %d" % (self.num_not_simd))
    print("# Constants: %d" % (self.num_consts))
    print("# Malicious: %d" % (self.num_mal))
