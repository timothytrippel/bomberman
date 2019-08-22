import sys

ifname = "or1200_fpu_div.dot"
ofname = "or1200_fpu_div.edited.dot"

ifile = open(ifname, 'r')
ofile = open(ofname, 'w')

for line in ifile:
	if "const." not in line:
		ofile.write(line)

ifile.close()
ofile.close()