import json
import os
import sys

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns


def load_data_df_wf(data_dir, clk_period, design_name):
    coal_counter_data = {
        'Design': [],
        'Time': [],
        'Total Coalesced TTTs': [],
        'Total Malicious Coalesced TTTs': [],
        'Coalesced Not Simd': [],
        'Coalesced Constants': [],
        'Coalesced Malicious': [],
    }

    dist_counter_data = {
        'Design': [],
        'Time': [],
        'Total Distributed TTTs': [],
        'Total Malicious Distributed TTTs': [],
        'Distributed Not Simd': [],
        'Distributed Constants': [],
        'Distributed Malicious': [],
    }

    # r=root, d=directories, f = files
    for r, d, f in os.walk(data_dir):
        for filename in f:
            if '.json' in filename and ('coal' in filename
                                        or 'dist' in filename):
                design_info = filename.split('.')
                time_limit = int(design_info[2])
                counter_type = design_info[1]

                # Open JSON file
                with open(data_dir + '/' + filename, 'r') as f:

                    # Read JSON file
                    json_dict = json.load(f)

                    # Get Coalesced Counter Data
                    if counter_type == 'coal':
                        coal_counter_data['Design'].append(design_name)
                        coal_counter_data['Time'].append(
                            float(time_limit) / float(clk_period))
                        coal_counter_data['Total Coalesced TTTs'].append(
                            int(json_dict['total']))
                        coal_counter_data['Coalesced Not Simd'].append(
                            int(json_dict['not_simd']))
                        coal_counter_data['Coalesced Constants'].append(
                            int(json_dict['constants']))
                        coal_counter_data['Coalesced Malicious'].append(
                            int(json_dict['malicious']))
                        coal_counter_data[
                            'Total Malicious Coalesced TTTs'].append(
                                int(json_dict['malicious']) +
                                int(json_dict['constants']))

                    # Get Distributed Counter Data
                    elif counter_type == 'dist':
                        dist_counter_data['Design'].append(design_name)
                        dist_counter_data['Time'].append(
                            float(time_limit) / float(clk_period))
                        dist_counter_data['Total Distributed TTTs'].append(
                            int(json_dict['total']))
                        dist_counter_data['Distributed Not Simd'].append(
                            int(json_dict['not_simd']))
                        dist_counter_data['Distributed Constants'].append(
                            int(json_dict['constants']))
                        dist_counter_data['Distributed Malicious'].append(
                            int(json_dict['malicious']))
                        dist_counter_data[
                            'Total Malicious Distributed TTTs'].append(
                                int(json_dict['malicious']) +
                                int(json_dict['constants']))

                    else:
                        print("ERROR: unknown counter type for file:",
                              filename)
                        sys.exit(1)

                f.close()

    # Convert data to a data frames and merge them
    coal_counter_df = pd.DataFrame(coal_counter_data)
    dist_counter_df = pd.DataFrame(dist_counter_data)
    counter_df = pd.merge(coal_counter_df,
                          dist_counter_df,
                          how='left',
                          on=['Time', 'Design'])
    return counter_df


def load_data_df_lf(data_dir):
    counter_data = {
        'Design': [],
        'Time': [],
        'Type': [],
        'Class': [],
        'Num. Counters': [],
    }

    # r=root, d=directories, f = files
    for r, d, f in os.walk(data_dir):
        for filename in f:
            if '.json' in filename and 'sizes' not in filename:
                design_info = filename.split('.')
                design_name = design_info[0]
                time_limit = int(design_info[2])
                counter_type = design_info[1]

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
                        counter_data['Num. Counters'].append(
                            int(json_dict['malicious']) +
                            int(json_dict['constants']))

                        # Constants
                        counter_data['Design'].append(design_name)
                        counter_data['Time'].append(time_limit)
                        counter_data['Type'].append('Coalesced')
                        counter_data['Class'].append('Constants')
                        counter_data['Num. Counters'].append(
                            int(json_dict['constants']))

                    # Get Distributed Counter Data
                    elif counter_type == 'dist':

                        # Total
                        counter_data['Design'].append(design_name)
                        counter_data['Time'].append(time_limit)
                        counter_data['Type'].append('Distributed')
                        counter_data['Class'].append('Total')
                        counter_data['Num. Counters'].append(
                            int(json_dict['malicious']) +
                            int(json_dict['constants']))

                        # Constants
                        counter_data['Design'].append(design_name)
                        counter_data['Time'].append(time_limit)
                        counter_data['Type'].append('Distributed')
                        counter_data['Class'].append('Constants')
                        counter_data['Num. Counters'].append(
                            int(json_dict['constants']))

                    else:
                        print("ERROR: unknown counter type for file:",
                              filename)
                        sys.exit(1)

                f.close()

    # Convert data to a data frames and merge them
    counter_df = pd.DataFrame(counter_data)
    return counter_df


def load_counter_sizes(data_dir):
    counter_sizes_data = {}

    # r=root, d=directories, f = files
    for r, d, f in os.walk(data_dir):
        for filename in f:
            if '.json' in filename and 'sizes' in filename:
                design_info = filename.split('.')
                counter_sizes_data['Design'] = design_info[0]

                # Open JSON file
                with open(data_dir + '/' + filename, 'r') as f:

                    # Read JSON file
                    json_dict = json.load(f)

                    # Get counter sizes
                    counter_sizes_data['Coalesced Sizes'] = json_dict[
                        'coal_sizes']
                    counter_sizes_data['Distributed Sizes'] = json_dict[
                        'dist_sizes']
                f.close()

    return counter_sizes_data


def plot_counter_df(fig_width,
                    fig_height,
                    counter_data_dir,
                    line_separators=[],
                    save_as_pdf=False,
                    pdf_fname='temp.pdf'):

    # Load Data
    counter_df = load_data_df_lf(counter_data_dir)

    # Plot Data
    fig, ax = plt.subplots(1, 1, figsize=(fig_width, fig_height), dpi=200)
    sns.lineplot(x="Time",
                 y="Num. Counters",
                 hue="Type",
                 style="Class",
                 data=counter_df,
                 ax=ax)
    ax.set_ylabel('# Malicious Counters')
    ax.set_xlabel('Time (ns)')
    plt.setp(ax.lines[0], linewidth=3)
    plt.setp(ax.lines[2], linewidth=3)
    ax.grid()
    plt.tight_layout(h_pad=1)

    # Add Line Separators
    for x_coord in line_separators:
        plt.axvline(x=x_coord, color='k', linestyle='-')

    # Save as PDF
    if save_as_pdf:
        plt.savefig(pdf_fname, format='pdf')
