# Holds two signals which are connected. Also, since this connection may
# occur on slices of the signal, we store additional msb and lsb values
class Connection:
  def __init__(self, sink_sig, sink_msb, sink_lsb, source_sig, source_msb,
               source_lsb):
    self.sink_sig = sink_sig
    self.sink_msb = sink_msb
    self.sink_lsb = sink_lsb
    self.source_sig = source_sig
    self.source_msb = source_msb
    self.source_lsb = source_lsb

  def __str__(self):
    out = self.source_sig.fullname() + "[" + str(self.source_msb) + ":" + str(
        self.source_lsb) + "]"
    out += " --> "
    out += self.sink_sig.fullname() + "[" + str(self.sink_msb) + ":" + str(
        self.sink_lsb) + "]"
    return out

  @classmethod
  def from_signals(cls, sink_signal, source_signal):
    cls(sink_signal, sink_signal.msb, sink_signal.lsb, source_signal,
        source_signal.msb, source_signal.lsb)
