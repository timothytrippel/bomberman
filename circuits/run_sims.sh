#!/bin/bash

#-------------------------------------------------------------------------------
# User-Defined Configurations
#-------------------------------------------------------------------------------

# Clock Basename (for IVL backend)
CLK_BASENAME='clk'

# Directories
BASE_DIR='/home/gridsan/TI27457/ttb'

# Resources
# PROCESSOR=opteron
PROCESSOR=xeon-e5
MEMORY=128gb
# MEMORY=64gb

# Design Configurations
# # AES
# DESIGN='aes'
# NUM_TESTS=50
# START_TIME=0
# TIME_LIMIT=-1
# TIME_RESOLUTION=100
# NUM_MALICIOUS_CNTRS=0

# # UART
# DESIGN='uart'
# NUM_TESTS=1
# START_TIME=0
# TIME_LIMIT=-1
# TIME_RESOLUTION=10000
# NUM_MALICIOUS_CNTRS=0

# OR1200
DESIGN='or1200'
NUM_TESTS=2
START_TIME=0
TIME_LIMIT=-1
TIME_RESOLUTION=100000
NUM_MALICIOUS_CNTRS=0
# PROGRAM_NUMS='0'
PROGRAM_NUMS='0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15'

# Flags
OVERWRITE_RESULTS=0
INTERACTIVE=0
LOGGING=1

#-------------------------------------------------------------------------------
# System Configurations (DO NOT EDIT)
#-------------------------------------------------------------------------------
HDL_BASE_DIR="${BASE_DIR}/circuits/${DESIGN}"
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
		pushd ${HDL_BASE_DIR} > /dev/null
		source ${SLURM_SCRIPTS_DIR}/gen_vcd.sbatch
		popd > /dev/null
	else

		# Run job non-interactively
		VCD_JOB_ID=$(\
			sbatch \
				-D ${HDL_BASE_DIR} \
				--job-name=${DESIGN}.vcd \
				--constraint=${PROCESSOR} \
				--dependency=afterok:${VVP_JOB_ID##* } \
				--export=LOGGING=${LOGGING},OUTPUT_DIR=${OUTPUT_DIR},DESIGN=${DESIGN},NUM_TESTS=${NUM_TESTS},PROGRAM_NUM=${PROGRAM_NUM} \
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

launch_tests() {

	# Set output directory
	OUTPUT_DIR="${HDL_BASE_DIR}/tjfree_${NUM_TESTS}tests-${TIME_RESOLUTION}res-100ps${OUT_DIR_TAG}-wprinting"

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
}

#-------------------------------------------------------------------------------
# Launch tests
#-------------------------------------------------------------------------------

# Launch tests based on design
case $DESIGN in

	aes)
		OUT_DIR_TAG=''
		launch_tests
	;;

	uart)
		OUT_DIR_TAG=''
		launch_tests
	;;

	or1200)
	
		# Iterate over OR1200 simulation programs
		for PROGRAM_NUM in ${PROGRAM_NUMS}; do

			case $PROGRAM_NUM in

				0)
					OUT_DIR_TAG='-helloworld'
				;;

				1)
					OUT_DIR_TAG='-aes'
				;;

				2)
					OUT_DIR_TAG='-basicmath'
				;;

				3)
					OUT_DIR_TAG='-blowfish'
				;;

				4)
					OUT_DIR_TAG='-crc'
				;;

				5)
					OUT_DIR_TAG='-dijkstra'
				;;

				6)
					OUT_DIR_TAG='-fft'
				;;

				7)
					OUT_DIR_TAG='-limits'
				;;

				8)
					OUT_DIR_TAG='-lzfx'
				;;

				9)
					OUT_DIR_TAG='-qsort'
				;;

				10)
					OUT_DIR_TAG='-randmath'
				;;

				11)
					OUT_DIR_TAG='-rc4'
				;;

				12)
					OUT_DIR_TAG='-rsa'
				;;

				13)
					OUT_DIR_TAG='-sha'
				;;

				14)
					OUT_DIR_TAG='-stringsearch'
				;;

				15)
					OUT_DIR_TAG='-susan'
				;;

				*)
					echo "ERROR: invalid program number name... abort."
					exit
				;;
			esac

			# Launch counter simulations and analysis
			launch_tests
			
		done
	;;

	*)
		echo "ERROR: invalid design name... abort."
		exit
	;;
esac

echo "Done."
