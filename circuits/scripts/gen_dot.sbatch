#!/bin/bash

#SBATCH --mail-type=NONE                       # Mail events (NONE, BEGIN, END, FAIL, ALL)
#SBATCH --mail-user=timothy.trippel@ll.mit.edu # Where to send mail	
#SBATCH --ntasks=1                             # Run on a single CPU
#SBATCH --mem=16gb                             # Job memory request
#SBATCH --time=03:00:00                        # Time limit hrs:min:sec
#SBATCH --output=/dev/null                     # Standard output and error log

# Return non-zero exit code if any command (in this script) fails
set -e

# Set sources, testbench, and includes
case $DESIGN in

	aes)
		SOURCES='aes_128.v round.v table.v'
		TESTBENCH='ttb_test_aes_128.v lfsr.v'
		INCLUDEDIRS=''
	;;

	uart)
		SOURCES='raminfr.v uart_receiver.v uart_regs.v uart_rfifo.v uart_sync_flops.v uart_tfifo.v uart_top.v uart_transmitter.v uart_wb.v'
		TESTBENCH='uart_test.v wb_mast.v lfsr.v'
		INCLUDEDIRS=''
	;;

	or1200)

		# RTL sub-directories
		ORPSOC_DIR='cores/top'
		CLKGEN_DIR='cores/clkgen'
		ARBITER_DIR='cores/arbiter'
		DBG_IF_DIR='cores/dbg_if'
		OR1200_DIR='cores/or1200'
		UART_DIR='cores/uart16550'
		RAM_DIR='cores/ram_wb'
		TESTBENCH_DIR='cores/bench'
		ORPSOC_RTL=`find ${ORPSOC_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		CLKGEN_RTL=`find ${CLKGEN_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		ARBITER_RTL=`find ${ARBITER_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		DBG_IF_RTL=`find ${DBG_IF_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		OR1200_RTL=`find ${OR1200_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		UART_RTL=`find ${UART_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		RAM_RTL=`find ${RAM_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		TESTBENCH=`find ${TESTBENCH_DIR} -type f -name "*.v" -not -name *._* -not -name *-defines.v -not -name *-params.v -not -name *_defines.v`
		SOURCES="${ORPSOC_RTL} ${CLKGEN_RTL} ${ARBITER_RTL} ${DBG_IF_RTL} ${OR1200_RTL} ${UART_RTL} ${RAM_RTL}"
		INCLUDEDIRS="-I${ORPSOC_DIR} -I${DBG_IF_DIR} -I${UART_DIR}  -I${DBG_IF_DIR} -I${OR1200_DIR} -I${TESTBENCH_DIR}"
	;;

	*)
		echo "ERROR: invalid design name... abort."
		exit
	;;
esac

if [[ $LOGGING -eq 1 ]]; then
	
	# Set dot log file
	DOT_LOG_FILE="${OUTPUT_DIR}/${DESIGN}.dot.log"

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
		-o ${OUTPUT_DIR}/${DESIGN}.dot \
		-pclk=${CLK_BASENAME} \
		-DVCD_FILENAME=\"${OUTPUT_DIR}/${DESIGN}.vcd\" \
		-t ttb \
		${INCLUDEDIRS} \
		${SOURCES} \
		${TESTBENCH}) >> ${DOT_LOG_FILE} 2>&1

else

	# Print status
	echo "Generating DOT graph..."
	echo "Design:      ${DESIGN}" 
	echo "Trojan Type: ${TTYPE}"  

	find . -type f -name "*.v" -not -name *._* -not -name test_aes_128.v

	# Generate DOT file (data flow graph)
	time iverilog \
		-o ${OUTPUT_DIR}/${DESIGN}.dot \
		-pclk=${CLK_BASENAME} \
		-DVCD_FILENAME=\"${OUTPUT_DIR}/${DESIGN}.vcd\" \
		-t ttb \
		${INCLUDEDIRS} \
		${SOURCES} \
		${TESTBENCH}
fi