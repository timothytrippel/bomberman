#!/bin/bash

#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --mem=16gb                             # Job memory request
#SBATCH --time=01:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

# Set dot log file
DOT_LOG_FILE="${OUTPUT_DIR}/${DESIGN}.${TTYPE}.dot.log"

# Print Job Info
pwd      >  ${DOT_LOG_FILE}
hostname >> ${DOT_LOG_FILE} 
date     >> ${DOT_LOG_FILE}

# Print status
echo "Generating DOT graph..." >> ${DOT_LOG_FILE}
echo "Design:      ${DESIGN}"  >> ${DOT_LOG_FILE}
echo "Trojan Type: ${TTYPE}"   >> ${DOT_LOG_FILE}

# Generate DOT file (data flow graph)
(time iverilog \
	-o ${DESIGN}.${TTYPE}.dot \
	-pclk=${CLK_BASENAME} \
	-DVCD_FILENAME=\"${DESIGN}.${TTYPE}.vcd\" \
	-t ttb \
	`find . -type f -name "*.v" -not -name test_aes_128.v`) >> ${DOT_LOG_FILE} 2>&1