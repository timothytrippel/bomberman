#!/bin/bash

#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --time=48:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

if [[ $LOGGING -eq 1 ]]; then
	
	# Set VCD log file
	VCD_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.vcd.log

	# Print Job Info
	pwd      >  ${VCD_LOG_FILE}
	hostname >> ${VCD_LOG_FILE} 
	date     >> ${VCD_LOG_FILE}

	# Print status
	echo "Generating VCD file with ${NUM_TESTS} test cases..." >> ${VCD_LOG_FILE}

	# Generate VCD file
	(time vvp \
		${OUTPUT_DIR}/${DESIGN}.vvp \
		-DVCD_FILENAME=\"${OUTPUT_DIR}/${DESIGN}.vcd\" \
		+num_tests=${NUM_TESTS}) >> ${VCD_LOG_FILE} 2>&1
else

	# Print status
	echo "Generating VCD file with ${NUM_TESTS} test cases..."

	# Generate VCD file
	time vvp \
		${OUTPUT_DIR}/${DESIGN}.vvp \
		-DVCD_FILENAME=\"${OUTPUT_DIR}/${DESIGN}.vcd\" \
		+num_tests=${NUM_TESTS}
fi