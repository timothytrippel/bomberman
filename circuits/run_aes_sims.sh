#!/bin/bash

#-------------------------------------------------------------------------------
# User-Defined Configurations
#-------------------------------------------------------------------------------

# Design
DESIGN='aes'
CLK_BASENAME='clk'

# Directories
BASE_DIR='/home/gridsan/TI27457/ttb'
OUTPUT_DIR="${BASE_DIR}/circuits/aes-logs"

# Flags
OVERWRITE_RESULTS=0

# HDL Sources
HDL_BASE_DIR="${BASE_DIR}/circuits/${DESIGN}"

# Trojan Types
TROJAN_TYPES='cdd'
# TROJAN_TYPES='cdd cdn cnd cnn ddd ddn dnd dnn tjfree'

# Test Range
NUM_TEST_INCREMENT=10
NUM_TESTS_RANGE=11
# NUM_TEST_INCREMENT=1000
# NUM_TESTS_RANGE=100001

# Flags
INTERACTIVE=0

#-------------------------------------------------------------------------------
# System Configurations (DO NOT EDIT)
#-------------------------------------------------------------------------------
SLURM_SCRIPTS_DIR="${BASE_DIR}/circuits/scripts"

#-------------------------------------------------------------------------------
# Functions
#-------------------------------------------------------------------------------

create_output_dir() {
	if [[ (-d "$OUTPUT_DIR") && (! $OVERWRITE_RESULTS) ]]; then
		echo "ERROR: output directory ${OUTPUT_DIR} already exists... abort."
		exit
	else
		mkdir -p ${OUTPUT_DIR}
	fi
}

gen_dot() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		source ${SLURM_SCRIPTS_DIR}/gen_dot.sbatch
	else

		# Run job non-interactively
		sbatch \
			-D ${HDL_BASE_DIR}/${TTYPE} \
			--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},CLK_BASENAME=${CLK_BASENAME} \
			${SLURM_SCRIPTS_DIR}/gen_dot.sbatch > /dev/null
	fi	
}

gen_vvp() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		source ${SLURM_SCRIPTS_DIR}/gen_vvp.sbatch
	else

		# Run job non-interactively
		VVP_JOB_ID=$(\
			sbatch \
			-D ${HDL_BASE_DIR}/${TTYPE} \
			--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE} \
			${SLURM_SCRIPTS_DIR}/gen_vvp.sbatch)
	fi	
}

gen_vcd() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		source ${SLURM_SCRIPTS_DIR}/gen_vcd.sbatch
	else

		# Run job non-interactively
		VCD_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR}/${TTYPE} \
				--dependency=afterok:${VVP_JOB_ID##* } \
				--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},NUM_TESTS=${NUM_TESTS} \
				${SLURM_SCRIPTS_DIR}/gen_vcd.sbatch)
	fi	
}

analyze_counters() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		source ${SLURM_SCRIPTS_DIR}/analyze_counters.sbatch
	else

		# Run job non-interactively
		sbatch \
			-D ${HDL_BASE_DIR}/${TTYPE} \
			--dependency=afterok:${VCD_JOB_ID##* } \
			--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},NUM_TESTS=${NUM_TESTS} \
			${SLURM_SCRIPTS_DIR}/analyze_counters.sbatch > /dev/null
	fi
}

#-------------------------------------------------------------------------------
# Launch tests
#-------------------------------------------------------------------------------

# Create Output Directory
echo "Creating output directory..."
create_output_dir

# Iterate over trojan types 
for TTYPE in ${TROJAN_TYPES}; do

	# Generate DOT file and VVP simulation binary
	echo "Starting CFG analysis of ${TTYPE} Trojans..."
	gen_dot
	# gen_vvp

	# # Launch test bench simulations
	# for (( NUM_TESTS=${NUM_TEST_INCREMENT}; NUM_TESTS<=${NUM_TESTS_RANGE}; NUM_TESTS += ${NUM_TEST_INCREMENT} )); do	

	# 	# Generate VCD file and analyze for counters
	# 	echo "Starting simulation with ${NUM_TESTS} tests ..."
	# 	gen_vcd
	# 	analyze_counters

	# done

done
echo "Done."
