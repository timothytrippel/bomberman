#!/bin/bash

#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --time=12:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

if [[ $LOGGING -eq 1 ]]; then

	# Set analysis log file
	ANALYSIS_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.${TTYPE}.${TIME_LIMIT}.ac.log

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
		${DESIGN}.${TTYPE}.dot \
		${DESIGN}.${TTYPE}.vcd \
		${OUTPUT_DIR}/${DESIGN}.${TTYPE}) >> ${ANALYSIS_LOG_FILE} 2>&1
else

	# Analyze design for counters
	time pypy \
		${OUTPUT_DIR}/../../scripts/analyze.py \
		${START_TIME} \
		${TIME_LIMIT} \
		${TIME_RESOLUTION} \
		${DESIGN}.${TTYPE}.dot \
		${DESIGN}.${TTYPE}.vcd \
		${OUTPUT_DIR}/${DESIGN}.${TTYPE}
fi