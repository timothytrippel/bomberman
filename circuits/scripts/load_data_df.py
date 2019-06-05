# Author:      Timothy Trippel
# Affiliation: MIT Lincoln Laboratory
# Email:       timothy.trippel@mit.ll.edu
# Description: 
# Script for loading counter analysis data into a Pandas dataframe for plotting.

import os
import sys
import json
import pandas as pd

def load_data_df(data_dir):
	counter_data = {
	    'Design'       : [],
	    'Time'         : [],
	    
	    'Total Coalesced Cntrs': [],
	    'Coalesced Not Simd'   : [],
	    'Coalesced Constants'  : [],
	    'Coalesced Malicious'  : [],

	    'Total Distributed Cntrs': [],
	    'Distributed Not Simd'   : [],
	    'Distributed Constants'  : [],
	    'Distributed Malicious'  : [],
	}

	# r=root, d=directories, f = files
	for r, d, f in os.walk(data_dir):
	    for filename in f:
	        if '.json' in filename and 'sizes' not in filename:
	            design_info  = filename.split('.')
	            design_name  = design_info[0]
	            time_limit   = int(design_info[2])
	            counter_type = design_info[1]
	            
	            # Open JSON file
	            with open(data_dir + '/' + filename, 'r') as f:

	                # Read JSON file
	                json_dict = json.load(f)
	                
	                # Get Coalesced Counter Data	
	                if counter_type == 'coal':
	                	counter_data['Design'].append(design_name)
	            		counter_data['Time'].append(time_limit)
	                	counter_data['Total Coalesced Cntrs'].append(json_dict['total'])
	                	counter_data['Coalesced Not Simd'].append(json_dict['not_simd'])
	                	counter_data['Coalesced Constants'].append(json_dict['constants'])
	                	counter_data['Coalesced Malicious'].append(json_dict['malicious'])

	                # Get Distributed Counter Data
	            	elif counter_type == 'dist':
	                	counter_data['Total Distributed Cntrs'].append(json_dict['total'])
	                	counter_data['Distributed Not Simd'].append(json_dict['not_simd'])
	                	counter_data['Distributed Constants'].append(json_dict['constants'])
	                	counter_data['Distributed Malicious'].append(json_dict['malicious'])

	                else:
	                	print "ERROR: unknown counter type for file:", filename
	                	sys.exit(1)

	            f.close()

	# print len(counter_data['Design'])
	# print len(counter_data['Time'])
	# print len(counter_data['Design'])
	# print len(counter_data['Design'])
	# print len(counter_data['Design'])


	# Conver data to a data frame            
	counter_df = pd.DataFrame(counter_data)
	return counter_df

def load_counter_sizes(data_dir):
	counter_sizes_data = {}

	# r=root, d=directories, f = files
	for r, d, f in os.walk(data_dir):
	    for filename in f:
	        if '.json' in filename and 'sizes' in filename:
	            design_info = filename.split('.')
	            counter_sizes_data['Design']       = design_info[0]
			
				# Open JSON file
	            with open(data_dir + '/' + filename, 'r') as f:

	                # Read JSON file
	                json_dict = json.load(f)
	                
	                # Get counter sizes
	                counter_sizes_data['Coalesced Sizes']   = json_dict['coal_sizes']
	                counter_sizes_data['Distributed Sizes'] = json_dict['dist_sizes']
	            f.close()

	return counter_sizes_data
