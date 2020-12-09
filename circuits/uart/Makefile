TARGET              := uart
TYPE                := tjfree
SOURCES             := raminfr.v \
	uart_receiver.v \
	uart_regs.v \
	uart_rfifo.v \
	uart_sync_flops.v \
	uart_tfifo.v \
	uart_top.v \
	uart_transmitter.v \
	uart_wb.v
TESTBENCH           := uart_test.v wb_mast.v lfsr.v
DUT_TOP_MODULE      := uart_test.uart1
NUM_TESTS           := 1
START_TIME          := 0
TIME_LIMIT          := -1
TIME_RESOLUTION     := 1000
NUM_MALICIOUS_CNTRS := 1
D_SIGNAL_BASENAME   := wb_clk_i
N_SIGNAL_BASENAME   := rf_count
OUT_DIR             := .

# Check if UART_DATA_SEED parameter set
ifndef UART_DATA_SEED
	UART_DATA_SEED := 0
endif

include ../common.mk
