#!/bin/bash

#-------------------------------------------------------------------------------
# User-Defined Configurations
#-------------------------------------------------------------------------------

# Design
DESIGNS='uart'
# DESIGNS='aes'
CLK_BASENAME='clk'

# Directories
BASE_DIR='/home/gridsan/TI27457/ttb'
LOGS_DIR_TAG=''

# Resources
# PROCESSOR=opteron
PROCESSOR=xeon-e5
MEMORY=128gb
# MEMORY=64gb

# Num Total (High Level) Test Cases
# AES    --> number of encryptions performed
# UART   --> number of sets of 16 bytes transmitted/received
# OR1200 --> number of (randomly selected) programs executed
# NUM_TESTS=50000
# NUM_TESTS=100
NUM_TESTS=1

# Counter Analysis Time Range 
# (in simulation timescale units)
START_TIME=0
# TIME_RESOLUTION=100
# START_TIME=5000
TIME_RESOLUTION=10000
# TIME_RESOLUTION=5000
TIME_LIMIT=-1

# Num Malcious Counters to Add
NUM_MALICIOUS_CNTRS=0

# Flags
OVERWRITE_RESULTS=0
INTERACTIVE=1
LOGGING=1

#-------------------------------------------------------------------------------
# System Configurations (DO NOT EDIT)
#-------------------------------------------------------------------------------
SLURM_SCRIPTS_DIR="${BASE_DIR}/circuits/scripts"

#-------------------------------------------------------------------------------
# Functions
#-------------------------------------------------------------------------------

create_output_dir() {
	if [[ (-d "$OUTPUT_DIR") && $OVERWRITE_RESULTS -eq 0 ]]; then
		echo "ERROR: output directory ${OUTPUT_DIR} already exists... abort."
		exit
	else
		mkdir -p ${OUTPUT_DIR}
	fi
}

gen_dot() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		pushd ${HDL_BASE_DIR} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/gen_dot.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		DOT_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR} \
				--job-name=${DESIGN}.dot \
				--constraint=opteron \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},CLK_BASENAME=${CLK_BASENAME} \
				${SLURM_SCRIPTS_DIR}/gen_dot.sbatch)
	fi	
}

gen_vvp() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		pushd ${HDL_BASE_DIR} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/gen_vvp.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		VVP_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR} \
				--job-name=${DESIGN}.vvp \
				--constraint=opteron \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN} \
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
				--job-name=${DESIGN}.vcd \
				--constraint=${PROCESSOR} \
				--dependency=afterok:${VVP_JOB_ID##* } \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},NUM_TESTS=${NUM_TESTS} \
				--mem=${MEMORY} \
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
			--job-name=${DESIGN}.${TIME_LIMIT}.ac \
			--constraint=${PROCESSOR} \
			--dependency=afterok:${DOT_JOB_ID##* },${VCD_JOB_ID##* } \
			--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},START_TIME=${START_TIME},TIME_LIMIT=${TIME_LIMIT},TIME_RESOLUTION=${TIME_RESOLUTION},NUM_MALICIOUS_CNTRS=${NUM_MALICIOUS_CNTRS} \
			--mem=${MEMORY} \
			${SLURM_SCRIPTS_DIR}/analyze_counters.sbatch > /dev/null
	fi
}

#-------------------------------------------------------------------------------
# Launch tests
#-------------------------------------------------------------------------------

# Iterate over designs
for DESIGN in ${DESIGNS}; do

	# Set HDL source directory
	HDL_BASE_DIR="${BASE_DIR}/circuits/${DESIGN}"

	# Set output directory
	OUTPUT_DIR="${BASE_DIR}/circuits/${DESIGN}-logs-${NUM_TESTS}tests-${TIME_RESOLUTION}res-100ps${LOGS_DIR_TAG}"

	# Create Output Directory
	echo "Creating output directory (${OUTPUT_DIR})..."
	create_output_dir

	# Generate DOT file 
	echo "Starting CFG analysis of ${DESIGN}..."
	gen_dot

	# Generate VVP simulation binary VCD file (i.e. run test bench simulation)
	echo "Starting simulation with ${NUM_TESTS} tests ..."
	gen_vvp
	gen_vcd

	# Analyze design for counters
	echo "Analyzing design for counters with time limit: ${TIME_LIMIT} ps ..."
	analyze_counters
done

echo "Done."
