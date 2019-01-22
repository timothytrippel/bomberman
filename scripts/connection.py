##
# Connection
##
# Holds two signals which are connected. Also, since this connection may
# occur on slices of the signal, I store additional msb and lsb values
class Connection:
	def __init__(self, dep_sig, d_msb, d_lsb, sig, msb, lsb):
		self.dep_sig = dep_sig
		self.d_msb = d_msb
		self.d_lsb = d_lsb
		self.sig = sig
		self.msb = msb
		self.lsb = lsb

	def __str__(self):
		out = self.dep_sig.fullname() + "[" + str(self.d_msb) + ":" + str(self.d_lsb) + "]"
		out += " <-- "
		out += self.sig.fullname() + "[" + str(self.msb) + ":" + str(self.lsb) + "]"
		return out
