SOURCES             := aes_128.v round.v table.v
# TESTBENCH           := test_aes_128.v
TESTBENCH           := ttb_test_aes_128.v lfsr.v
TARGET              := aes
DUT_TOP_MODULE      := ttb_test_aes_128.dut
NUM_TESTS           := 75
START_TIME          := 0
TIME_LIMIT          := -1
TIME_RESOLUTION     := 100
NUM_MALICIOUS_CNTRS := 1
D_SIGNAL_BASENAME   := clk
N_SIGNAL_BASENAME   := state

include ../common.mk
