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

	# Set VCD log file
	VVP_LOG_FILE=${OUTPUT_DIR}/${DESIGN}.vvp.log

	# Print Job Info
	pwd      >  ${VVP_LOG_FILE}
	hostname >> ${VVP_LOG_FILE} 
	date     >> ${VVP_LOG_FILE}

	# Print status
	echo "Generating VVP simulation binary..." >> ${VVP_LOG_FILE}

	# Generate (VVP) simulation binary
	(time iverilog \
		-t vvp \
		-o ${OUTPUT_DIR}/${DESIGN}.vvp \
		-DVCD_FILENAME=\"${OUTPUT_DIR}/${DESIGN}.vcd\" \
		${INCLUDEDIRS} \
		${SOURCES} \
		${TESTBENCH}) >> ${VVP_LOG_FILE} 2>&1
else

	# Print status
	echo "Generating VVP simulation binary..."

	# Generate (VVP) simulation binary
	time iverilog \
		-t vvp \
		-o ${OUTPUT_DIR}/${DESIGN}.vvp \
		-DVCD_FILENAME=\"${OUTPUT_DIR}/${DESIGN}.vcd\" \
		${INCLUDEDIRS} \
		${SOURCES} \
		${TESTBENCH}
fi