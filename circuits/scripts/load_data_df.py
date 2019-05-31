# Author:      Timothy Trippel
# Affiliation: MIT Lincoln Laboratory
# Email:       timothy.trippel@mit.ll.edu
# Description: 
# Script for loading counter analysis data into a Pandas dataframe for plotting.

import os
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
	            design_info = filename.split('.')
	            counter_data['Design'].append(design_info[0])
	            counter_data['Time'].append(int(design_info[1]))

	            # Open JSON file
	            with open(data_dir + '/' + filename, 'r') as f:

	                # Read JSON file
	                json_dict = json.load(f)
	                
	                # Get Coalesced Counter Data
	                counter_data['Total Coalesced Cntrs'].append(json_dict['coal_total'])
	                counter_data['Coalesced Not Simd'].append(json_dict['coal_not_simd'])
	                counter_data['Coalesced Constants'].append(json_dict['coal_constants'])
	                counter_data['Coalesced Malicious'].append(json_dict['coal_malicious'])

	                # Get Distributed Counter Data
	                counter_data['Total Distributed Cntrs'].append(json_dict['dist_total'])
	                counter_data['Distributed Not Simd'].append(json_dict['dist_not_simd'])
	                counter_data['Distributed Constants'].append(json_dict['dist_constants'])
	                counter_data['Distributed Malicious'].append(json_dict['dist_malicious'])
	            f.close()

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
