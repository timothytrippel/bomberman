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


class Connection:
  """Holds two signals which are connected.

  Since this connection may occur on slices of the signal, we store additional
  MSB and LSB values.
  """
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
