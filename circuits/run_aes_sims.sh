#!/bin/bash

#-------------------------------------------------------------------------------
# Configurations
#-------------------------------------------------------------------------------

# Design
DESIGN='aes'
CLK_BASENAME='clk'

# Directories
BASE_DIR='/home/gridsan/TI27457/ttb'
OUTPUT_DIR="${BASE_DIR}/circuits/aes-logs"

# Flags
OVERWRITE_RESULTS=0

# HDL Sources
HDL_BASE_DIR="${BASE_DIR}/circuits/${DESIGN}"
TOP_MODULE='ttb_test_aes_128'
DESIGN_HDL='aes_128.v round.v table.v counter.v'
TESTBENCH_HDL='ttb_test_aes_128.v lfsr.v'

# Trojan Types
# TROJAN_TYPES='cdd'
TROJAN_TYPES='cdd cdn cnd cnn ddd ddn dnd dnn tjfree'

# Test Range
NUM_TEST_INCREMENT=1000
NUM_TESTS_RANGE=10000001
# NUM_TESTS_RANGE=10000001

#-------------------------------------------------------------------------------
# Launch tests
#-------------------------------------------------------------------------------

# Create Output Directory
echo "Creating output directory..."
if [[ (-d "$OUTPUT_DIR") && (! $OVERWRITE_RESULTS) ]]; then
	echo "ERROR: output directory ${OUTPUT_DIR} already exists... abort."
	exit
else
	mkdir -p ${OUTPUT_DIR}
fi

# Iterate over trojan types 
for TTYPE in ${TROJAN_TYPES}; do

	echo "Starting analysis of ${TTYPE} Trojans..."

	# Set static analysis log file
	STA_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.${TTYPE}.log

	echo "Design:      ${DESIGN}" >  ${STA_LOG_FILE}
	echo "Trojan Type: ${TTYPE}"  >> ${STA_LOG_FILE}

	# Switch to HDL directory
	pushd ${HDL_BASE_DIR}/${TTYPE} > /dev/null

	# Launch DOT file generation
	echo "Generating DOT graph..." >> ${STA_LOG_FILE}
	(time iverilog \
		-o ${DESIGN}-${TTYPE}.dot \
		-pclk=${CLK_BASENAME} \
		-t ttb \
		-s ${TOP_MODULE} \
		${TESTBENCH_HDL} \
		${DESIGN_HDL}) >> ${STA_LOG_FILE} 2>&1

	# Launch VVP file generation
	echo "Generating VVP simulation binary..." >> ${STA_LOG_FILE}
	(time iverilog \
		-t vvp \
		-o ${DESIGN}-${TTYPE}.vvp \
		${DESIGN_HDL} \
		${TESTBENCH_HDL}) >> ${STA_LOG_FILE} 2>&1

	# Launch test bench simulations
	for (( NUM_TESTS=${NUM_TEST_INCREMENT}; NUM_TESTS<=${NUM_TESTS_RANGE}; NUM_TESTS += ${NUM_TEST_INCREMENT} )); do	

		echo "Starting simulation with ${NUM_TESTS} tests ..."

		# Set dynamic analysis log file
		DNA_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.${TTYPE}.${NUM_TESTS}.log

		# Launch VCD file generation
		echo "Generating VCD file with ${NUM_TESTS} test cases..." >> ${DNA_LOG_FILE}
		(time vvp \
			${DESIGN}-${TTYPE}.vvp \
			+num_tests=${NUM_TESTS}) >> ${DNA_LOG_FILE} 2>&1

		# Launch counter analysis
		echo "Launching counter analysis..." >> ${DNA_LOG_FILE}
		(time pypy \
			${BASE_DIR}/scripts/analyze.py \
			${DESIGN}-${TTYPE}.dot \
			${DESIGN}-${TTYPE}.vcd) >> ${DNA_LOG_FILE} 2>&1
	done

	# Switch back to top directory
	popd > /dev/null

done
echo "Done."