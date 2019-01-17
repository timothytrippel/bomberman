import sys

# This library was found online
from Verilog_VCD import parse_vcd

# Looks for unique values in a list
#
# If there is a repeated value we return 0
# Otherwise we return the number of items in the list
# @TODO: There must be an easier way to do this
def unique_values(l):
	seen = set()
	for item in l:
		if item[1] not in seen:
			# Necessary for 'x' or 'z' values
			if item[1].isdigit():
				# New value, add to set
				seen.add(item[1])
		else:
			# There was a repeat, return 0
			return 0
	return len(seen)

if (len(sys.argv) != 2):
	sys.stderr.write("usage: VCD_parse.py {filename}\n")
	sys.exit(1)

print "Parsing VCD..."
vcd = parse_vcd(sys.argv[1], types={"reg", "wire"})
print "Anaylzing VCD..."

num_signals = 0
num_malic_signals = 0;

for symbol, data in vcd.iteritems():
	# All regs and any created wires
	if not data["nets"][0]["type"] == "reg" and "_ttb" not in data["nets"][0]["name"]: 
		continue

	num_signals += 1

	num_unique_values = unique_values(data["tv"]) 
	max_num_unique_values = 2 ** int(data["nets"][0]["size"])

	# num_unique_values != 0 checks for a repeated value
	# num_unique_values != max_num_unique_values checks that all values were enumerated
	if num_unique_values != 0 and num_unique_values != max_num_unique_values:
		name = data["nets"][0]["hier"] + "." + data["nets"][0]["name"]
		if num_unique_values == 1:
			print "Constant: " + name
		else:
			print "Possible Malicious Symbol: " + data["nets"][0]["hier"] + "." + data["nets"][0]["name"]
			num_malic_signals += 1
print "Malicous Marked: " + str(num_malic_signals) + "/" + str(num_signals)  
