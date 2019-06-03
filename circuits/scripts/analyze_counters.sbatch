#!/bin/bash

#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --time=72:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

# Set top module of DUT
case $DESIGN in

	aes)
		DUT_TOP_MODULE="ttb_test_aes_128.dut"
	;;

	uart)
		DUT_TOP_MODULE="uart_test.uart1"
	;;

	or1200)
		DUT_TOP_MODULE="or1200"
	;;

	*)
		echo "ERROR: invalid design name... abort."
		exit
	;;
esac

if [[ $LOGGING -eq 1 ]]; then

	# Set analysis log file
	ANALYSIS_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.${TIME_LIMIT}.ac.log

	# Print Job Info
	pwd      >  ${ANALYSIS_LOG_FILE}
	hostname >> ${ANALYSIS_LOG_FILE} 
	date     >> ${ANALYSIS_LOG_FILE}

	# Analyze design for counters
	(time pypy \
		${OUTPUT_DIR}/../../scripts/analyze.py \
		${START_TIME} \
		${TIME_LIMIT} \
		${TIME_RESOLUTION} \
		${DUT_TOP_MODULE} \
		${NUM_MALICIOUS_CNTRS} \
		${OUTPUT_DIR}/${DESIGN}.dot \
		${OUTPUT_DIR}/${DESIGN}.vcd \
		${OUTPUT_DIR}/${DESIGN}) >> ${ANALYSIS_LOG_FILE} 2>&1
else

	# Analyze design for counters
	time pypy \
		${OUTPUT_DIR}/../../scripts/analyze.py \
		${START_TIME} \
		${TIME_LIMIT} \
		${TIME_RESOLUTION} \
		${DUT_TOP_MODULE} \
		${NUM_MALICIOUS_CNTRS} \
		${OUTPUT_DIR}/${DESIGN}.dot \
		${OUTPUT_DIR}/${DESIGN}.vcd \
		${OUTPUT_DIR}/${DESIGN}
fi