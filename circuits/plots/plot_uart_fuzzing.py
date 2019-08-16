import sys
sys.path.insert(0, '../scripts')
from load_data_df import *
import matplotlib.gridspec as gridspec
from mpl_toolkits.axes_grid1.inset_locator import zoomed_inset_axes, inset_axes
from mpl_toolkits.axes_grid1.inset_locator import mark_inset

# Define Design Characteristics
uart_clk_period    = 10
uart_ylim          = 150
uart_num_trials    = 25
root_uart_data_dir = '/Users/ti27457/Repos/ttb/circuits/uart/tjfree_16bytes_seed'

# List of data frames
uart_dfs = []

# Load dataframes
for i in range(uart_num_trials):
	print "Loading seed %d data..." % (i)
	uart_dfs.append(load_data_df_wf(root_uart_data_dir + str(i), uart_clk_period, 'uart_seed' + str(i)))
	uart_dfs[i].to_csv('uart_df_seed' + str(i) + '.csv', index=False)
print "Done."

# Concatenate dataframes
merged_uart_df = pd.concat(uart_dfs)

# Save merged dataframe
merged_uart_df.to_csv('merged_uart_df.csv', index=False)

# # Plot Settings
# LINE_WIDTH = 2

# # Create Figure
# sns.set()
# fig, ax = plt.subplots(1, 1, figsize=(9, 3))

# # Plot Data
# for i in range(uart_num_trials):
# 	sns.lineplot(x="Time", y="Total Malicious Coalesced TTTs",   data=uart_dfs[i], ax=ax, linewidth=LINE_WIDTH, estimator=None)
# 	sns.lineplot(x="Time", y="Total Malicious Distributed TTTs", data=uart_dfs[i], ax=ax, linewidth=LINE_WIDTH, estimator=None)

# # Format Main Plot
# ax.set_ylim(-10, uart_ylim)
# ax.set_xlim(10050, 740306)
# # ax.set_xlim(180000, 200000)
# ax.set_xlabel('Clock Cycles')
# ax.set_ylabel('# TTTs')
# for tick in ax.get_yticklabels():
# 	tick.set_rotation(90)

# Save Plot as PNG
# plt.show()
# plt.savefig("uart_fuzzing_2.png", format='png', bbox_inches='tight', transparent=False)