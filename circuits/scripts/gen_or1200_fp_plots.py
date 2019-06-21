import os
import sys
import json
sys.path.insert(0, '/home2/gridsan/TI27457/.local/lib/python2.7/site-packages/')
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

BASE_OUTPUT_DIR = "../or1200/tjfree_2tests-100000res-100ps-"
# OR1200_PROGRAMS = [\
# 	"helloworld", \
# 	"aes", \
# 	"crc", \
# 	"limits", \
# 	"randmath", \
# 	"rc4", \
# 	"rsa" \
# ]
OR1200_PROGRAMS = [\
	"helloworld", \
]

def load_data_df_lf(data_dir):
	counter_data = {
	    'Design'        : [],
	    'Time'          : [],
	    'Type'          : [],
	    'Class'         : [],
	    'Num. Counters' : [],
	}

	# r=root, d=directories, f = files
	for r, d, f in os.walk(data_dir):
	    for filename in f:
	        if '.json' in filename and 'sizes' not in filename:
	            design_info  = filename.split('.')
	            design_name  = design_info[0]
	            time_limit   = int(design_info[2])
	            counter_type = design_info[1]
	            # print "Design:      ", design_name
	            # print "Time Limit:  ", time_limit
	            # print "Counter Type:", counter_type

	            # Open JSON file
	            with open(data_dir + '/' + filename, 'r') as f:

	                # Read JSON file
	                json_dict = json.load(f)
	                
	                # Get Coalesced Counter Data
	                if counter_type == 'coal':

						# Total
						counter_data['Design'].append(design_name)
						counter_data['Time'].append(time_limit)
						counter_data['Type'].append('Coalesced')
						counter_data['Class'].append('Total')
						counter_data['Num. Counters'].append(int(json_dict['malicious']) + int(json_dict['constants']))

						# Constants
						counter_data['Design'].append(design_name)
						counter_data['Time'].append(time_limit)
						counter_data['Type'].append('Coalesced')
						counter_data['Class'].append('Constants')
						counter_data['Num. Counters'].append(int(json_dict['constants']))

	                # Get Distributed Counter Data
	            	elif counter_type == 'dist':
	            		
						# Total
						counter_data['Design'].append(design_name)
						counter_data['Time'].append(time_limit)
						counter_data['Type'].append('Distributed')
						counter_data['Class'].append('Total')
						counter_data['Num. Counters'].append(int(json_dict['malicious']) + int(json_dict['constants']))

						# Constants
						counter_data['Design'].append(design_name)
						counter_data['Time'].append(time_limit)
						counter_data['Type'].append('Distributed')
						counter_data['Class'].append('Constants')
						counter_data['Num. Counters'].append(int(json_dict['constants']))

	                else:
	                	print "ERROR: unknown counter type for file:", filename
	                	sys.exit(1)

	            f.close()

	# Convert data to a data frames and merge them           
	counter_df = pd.DataFrame(counter_data)
	return counter_df

def get_line_separators(vcd_log):
	line_seps = []
	with open(vcd_log, "r") as stream:
		for line in stream:
			if "DUT Reset Complete" in line:
				# print line
				line_seps.append(float(line.split()[-2]) * 100)
			elif "Simulation Complete" in line:
				# print line
				line_seps.append(float(line.split()[-2]) * 100)
	stream.close()
	return line_seps[1:]

def plot_counter_df(fig_width, fig_height, counter_data_dir, program, line_separators=[], save_as_pdf=False, pdf_fname='temp.pdf'):
	
	# Load Data
	counter_df = load_data_df_lf(counter_data_dir)

	# Plot Data
	fig, ax = plt.subplots(1, 1, figsize=(fig_width, fig_height), dpi=200)
	sns.lineplot(x="Time", y="Num. Counters", hue="Type", style="Class", data=counter_df, ax=ax)
	ax.set_ylabel('# Malicious Counters')
	ax.set_xlabel('Time (ns)')
	ax.set_title(program)
	plt.setp(ax.lines[0], linewidth=5)
	plt.setp(ax.lines[2], linewidth=5)
	ax.grid()
	plt.tight_layout(h_pad=1)

	# Add Line Separators
	for x_coord in line_separators:
		plt.axvline(x=x_coord, color='k', linestyle='-')

	# Save as PDF
	if save_as_pdf:
		plt.savefig(pdf_fname, format='pdf')

for program in OR1200_PROGRAMS:
	print "Generating plot for:", program
	data_dir  = BASE_OUTPUT_DIR + program + "-wprinting"
	line_seps = get_line_separators(data_dir + "/or1200.vcd.log")
	plot_counter_df(\
		12, \
		6, \
		data_dir, \
		program, \
		line_separators=line_seps, \
		save_as_pdf=True, \
		pdf_fname="../plots/or1200_" + program + "_2x.pdf")