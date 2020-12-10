SOURCES             := aes_128.v round.v table.v
# TESTBENCH           := test_aes_128.v
TESTBENCH           := ttb_test_aes_128.v lfsr.v
TARGET              := aes
DUT_TOP_MODULE      := ttb_test_aes_128.dut
NUM_TESTS           := 75
START_TIME          := 0
TIME_LIMIT          := -1
TIME_RESOLUTION     := 100
NUM_MALICIOUS_CNTRS := 0
D_SIGNAL_BASENAME   := clk
N_SIGNAL_BASENAME   := state
OUT_DIR             := .

# Check if AES_KEY_SEED parameter set
ifndef AES_KEY_SEED
	AES_KEY_SEED := CAFEFEEDCAFEFEEDCAFEFEEDCAFEFEED
endif

# Check if AES_STATE_SEED parameter set
ifndef AES_STATE_SEED
	AES_STATE_SEED := DEADBEEFDEADBEEFDEADBEEFDEADBEEF
endif

include ../common.mk
