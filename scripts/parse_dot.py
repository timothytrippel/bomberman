import re
import sys

#
# Signal
#
# Holds all the info for a signal
class Signal:
  def __init__(self, name, msb, lsb):
    self.name = name
    self.lsb  = lsb
    self.msb  = msb
    self.isff = False
    self.isinput = False
    self.conn = []

  def __str__(self):
    return self.name + "[" + str(self.msb) + ":" + str(self.lsb) + "]"

  def fullname(self):
    return self.name

  def connections(self):
    return self.conn;

  def addconn(self, c):
    self.conn.append(c)

#
# Connection
#
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

# Builds out the dependencies for each signal passed in
# @TODO: Currently inefficient, re-computing a lot of things
def build_deps(sig, msb, lsb, ffs = [], seen = {}):
  # If we've seen this exact slice before, continue
  if str(sig) + str(msb) + str(lsb) in seen.keys():
    return ffs

  # Mark this signal as seen
  seen[str(sig) + str(msb) + str(lsb)] = True

  # If its a FF or input, it is the end
  if (sig.isff or sig.isinput):

    # Make sure we aren't already inserted
    for ff in ffs:
      if str(ff) == str(sig):
        return ffs

    
    ffs.append(Signal(sig.fullname(), msb, lsb))
    return ffs

  for c in sig.conn:
    build_deps(c.sig, c.msb, c.lsb, ffs, seen)
  return ffs

def parse_file(file_name):
  with open(file_name, 'r') as f:
    read_contents = f.read()

  lines = read_contents.splitlines()

  if "digraph G {" not in lines[0]:
    sys.stderr.write("Error: Invalid file format\n")
    sys.exit(1)

  lineno    =  1
  signals   = {}

  if len(lines) >= (1 << 20):
    sys.stderr.write("Error: Over a million signals. Too many to handle\n")
    sys.exit(1)

  while "->" not in lines[lineno] and lineno < len(lines):
    m = re.search('"[\w\.]+" \[shape=([A-Za-z]+), label="([\w\.]+)\[(\d+):(\d+)\]"\]', lines[lineno])
    if (None == m):
      print "Error: Bad format at line " + str(lineno)
      sys.exit(2)

    lineno += 1
    s = Signal(m.group(2), int(m.group(3)), int(m.group(4)))

    if m.group(1) == "none":
      s.isinput = True
    elif m.group(1) == "square":
      s.isff = True

    signals[s.fullname()] = s

  while "}" not in lines[lineno] and lineno < len(lines):
    m = re.search('"([\w\.]+)" -> "([\w\.]+)"\[label="\[(\d+):(\d+)\]->\[(\d+):(\d+)\]"\]', lines[lineno])

    if None == m:
      sys.stderr.write("Error: Invalid file format\n")
      sys.exit(1)

    dep_sig = signals[m.group(2)]
    d_msb   = int(m.group(5))
    d_lsb   = int(m.group(6))

    sig     = signals[m.group(1)]
    msb     = int(m.group(3))
    lsb     = int(m.group(4))

    c = Connection(dep_sig, d_msb, d_lsb, sig, msb, lsb)
    signals[dep_sig.fullname()].addconn(c)

    lineno += 1

  assert "}" in lines[lineno]
  assert lineno + 1 == len(lines)

  return signals

def generate_distributed_counters(signals):
  deps = []
  seen = {}
  for sig_name, sig in signals.iteritems():
    ffs = build_deps(sig, sig.msb, sig.lsb, [], {})
    if len(ffs) == 0:
      continue

    if len(ffs) == 1:
      if ffs[0].fullname() in signals.keys():
        continue

    ffs.sort(key=str)
    # Stringfy 
    distr = ','.join([str(ff) for ff in ffs])
    if distr not in seen:
      seen[distr] = True
      deps.append(ffs)
  return deps
