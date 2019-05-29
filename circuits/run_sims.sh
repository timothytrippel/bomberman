#!/bin/bash

#-------------------------------------------------------------------------------
# User-Defined Configurations
#-------------------------------------------------------------------------------

# Design
# DESIGNS='uart'
DESIGNS='aes'
CLK_BASENAME='clk'

# Directories
BASE_DIR='/home/gridsan/TI27457/ttb'
LOGS_DIR_TAG='logs'
# LOGS_DIR_TAG='logs-50ktests-100res-100ps'

# Resources
# PROCESSOR=opteron
PROCESSOR=xeon-e5
MEMORY=128gb
# MEMORY=64gb

# Trojan Types
TROJAN_TYPES='cdd'
# TROJAN_TYPES='tjfree'
# TROJAN_TYPES='cdd cdn cnd cnn ddd ddn dnd dnn tjfree'

# Num Total (High Level) Test Cases
# AES    --> number of encryptions performed
# UART   --> number of sets of 16 bytes transmitted/received
# OR1200 --> number of (randomly selected) programs executed
# NUM_TESTS=100000
# NUM_TESTS=50000
NUM_TESTS=50
# NUM_TESTS=16

# Counter Analysis Time Range
START_TIME=0
# START_TIME=5000
TIME_RESOLUTION=100
# TIME_RESOLUTION=5000
TIME_LIMIT=-1

# Num Malcious Counters to Add
NUM_MALICIOUS_CNTRS=0

# Flags
OVERWRITE_RESULTS=0
INTERACTIVE=1
LOGGING=0

#-------------------------------------------------------------------------------
# System Configurations (DO NOT EDIT)
#-------------------------------------------------------------------------------
SLURM_SCRIPTS_DIR="${BASE_DIR}/circuits/scripts"

#-------------------------------------------------------------------------------
# Functions
#-------------------------------------------------------------------------------

create_output_dir() {
	if [[ (-d "$OUTPUT_DIR") && $OVERWRITE_RESULTS ]]; then
		echo "ERROR: output directory ${OUTPUT_DIR} already exists... abort."
		exit
	else
		mkdir -p ${OUTPUT_DIR}
	fi
}

gen_dot() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		pushd ${HDL_BASE_DIR}/${TTYPE} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/gen_dot.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		DOT_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR}/${TTYPE} \
				--job-name=${DESIGN}.${TTYPE}.dot \
				--constraint=opteron \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},CLK_BASENAME=${CLK_BASENAME} \
				${SLURM_SCRIPTS_DIR}/gen_dot.sbatch)
	fi	
}

gen_vvp() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		pushd ${HDL_BASE_DIR}/${TTYPE} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/gen_vvp.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		VVP_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR}/${TTYPE} \
				--job-name=${DESIGN}.${TTYPE}.vvp \
				--constraint=opteron \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE} \
				${SLURM_SCRIPTS_DIR}/gen_vvp.sbatch)
	fi	
}

gen_vcd() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		pushd ${HDL_BASE_DIR}/${TTYPE} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/gen_vcd.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		VCD_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR}/${TTYPE} \
				--job-name=${DESIGN}.${TTYPE}.vcd \
				--constraint=${PROCESSOR} \
				--dependency=afterok:${VVP_JOB_ID##* } \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},NUM_TESTS=${NUM_TESTS} \
				--mem=${MEMORY} \
				${SLURM_SCRIPTS_DIR}/gen_vcd.sbatch)
	fi	
}

analyze_counters() {
	if [[ $INTERACTIVE -eq 1 ]]; then

		# Run job interactively
		pushd ${HDL_BASE_DIR}/${TTYPE} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/analyze_counters.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		sbatch \
			-D ${HDL_BASE_DIR}/${TTYPE} \
			--job-name=${DESIGN}.${TTYPE}.${TIME_LIMIT}.ac \
			--constraint=${PROCESSOR} \
			--dependency=afterok:${DOT_JOB_ID##* },${VCD_JOB_ID##* } \
			--export=LOGGING=${LOGGING},\
			OUTPUT_DIR=${OUTPUT_DIR},\
			DESIGN=${DESIGN},\
			TTYPE=${TTYPE},\
			START_TIME=${START_TIME},TIME_LIMIT=${TIME_LIMIT},TIME_RESOLUTION=${TIME_RESOLUTION},NUM_MALICIOUS_CNTRS=${NUM_MALICIOUS_CNTRS} \
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
	OUTPUT_DIR="${BASE_DIR}/circuits/${DESIGN}-${LOGS_DIR_TAG}"

	# Create Output Directory
	echo "Creating output directory (${OUTPUT_DIR})..."
	create_output_dir

	# Iterate over trojan types 
	for TTYPE in ${TROJAN_TYPES}; do

		# # Generate DOT file 
		# echo "Starting CFG analysis of ${TTYPE} Trojans..."
		# gen_dot

		# # Generate VVP simulation binary VCD file (i.e. run test bench simulation)
		# echo "Starting simulation with ${NUM_TESTS} tests ..."
		# gen_vvp
		# gen_vcd

		# Analyze design for counters
		echo "Analyzing design for counters with time limit: ${TIME_LIMIT} ps ..."
		analyze_counters
	done
done

echo "Done."
