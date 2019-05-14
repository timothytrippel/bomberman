#!/bin/bash

#SBATCH --job-name=analyze_counters            # Job name
#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --mem=64gb                             # Job memory request
#SBATCH --time=12:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

# Set analysis log file
ANALYSIS_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.${TTYPE}.${NUM_TESTS}.ac.log

# Print Job Info
pwd      >  ${ANALYSIS_LOG_FILE}
hostname >> ${ANALYSIS_LOG_FILE} 
date     >> ${ANALYSIS_LOG_FILE}

# Analyze design for counters
(time pypy \
	${OUTPUT_DIR}/../../scripts/analyze.py \
	${DESIGN}-${TTYPE}.dot \
	${DESIGN}-${TTYPE}.vcd) >> ${ANALYSIS_LOG_FILE} 2>&1