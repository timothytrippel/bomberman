#!/bin/bash

#-------------------------------------------------------------------------------
# User-Defined Configurations
#-------------------------------------------------------------------------------

# Design
DESIGNS='uart'
CLK_BASENAME='clk'

# Directories
BASE_DIR='/home/gridsan/TI27457/ttb'

# Resources
PROCESSOR=opteron
# PROCESSOR=xeon-e5
MEMORY=64gb

# Flags
OVERWRITE_RESULTS=0
INTERACTIVE=0

# Trojan Types
TROJAN_TYPES='cdd'
# TROJAN_TYPES='cdd cdn cnd cnn ddd ddn dnd dnn tjfree'

# Test Range
NUM_TEST_START=1
NUM_TEST_INCREMENT=1
NUM_TESTS_RANGE=1
# NUM_TEST_START=1000
# NUM_TEST_INCREMENT=50
# NUM_TESTS_RANGE=2000

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
				--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},CLK_BASENAME=${CLK_BASENAME} \
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
			--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},NUM_TESTS=${NUM_TESTS} \
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
				--job-name=${DESIGN}.${TTYPE}.${NUM_TESTS}.vcd \
				--constraint=${PROCESSOR} \
				--dependency=afterok:${VVP_JOB_ID##* } \
				--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},NUM_TESTS=${NUM_TESTS} \
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
			--job-name=${DESIGN}.${TTYPE}.${NUM_TESTS}.ac \
			--constraint=${PROCESSOR} \
			--dependency=afterok:${DOT_JOB_ID##* },${VCD_JOB_ID##* } \
			--export=OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},TTYPE=${TTYPE},NUM_TESTS=${NUM_TESTS} \
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
	OUTPUT_DIR="${BASE_DIR}/circuits/${DESIGN}-logs"

	# Create Output Directory
	echo "Creating output directory (${OUTPUT_DIR})..."
	create_output_dir

	# Iterate over trojan types 
	for TTYPE in ${TROJAN_TYPES}; do

		# Generate DOT file and VVP simulation binary
		echo "Starting CFG analysis of ${TTYPE} Trojans..."
		gen_dot

		# Launch test bench simulations
		for (( NUM_TESTS=${NUM_TEST_START}; NUM_TESTS<=${NUM_TESTS_RANGE}; NUM_TESTS += ${NUM_TEST_INCREMENT} )); do	

			# Generate VCD file and analyze for counters
			echo "Starting simulation with ${NUM_TESTS} tests ..."
			gen_vvp
			gen_vcd
			analyze_counters

		done
	done
done

echo "Done."
