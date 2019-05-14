#!/bin/bash

#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --mem=16gb                             # Job memory request
#SBATCH --time=01:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

# Set VCD log file
VVP_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.${TTYPE}.vvp.log

# Print Job Info
pwd      >  ${VVP_LOG_FILE}
hostname >> ${VVP_LOG_FILE} 
date     >> ${VVP_LOG_FILE}

# Print status
echo "Generating VVP simulation binary..." >> ${VVP_LOG_FILE}

# Generate (VVP) simulation binary
(time iverilog \
	-t vvp \
	-o ${DESIGN}-${TTYPE}.vvp \
	`find . -type f -name "*.v" -not -name test_aes_128.v`) >> ${VVP_LOG_FILE} 2>&1