export SOURCES             := ../../third_party/picorv32/picorv32.v
export TESTBENCH           := ../../third_party/picorv32/testbench.v
export TARGET              := testbench
export DUT_TOP_MODULE      := testbench.top.uut
export NUM_TESTS           := 2
export START_TIME          := 0
export TIME_LIMIT          := -1
export TIME_RESOLUTION     := 10000
export NUM_MALICIOUS_CNTRS := 0
export D_SIGNAL_BASENAME   := clk
export N_SIGNAL_BASENAME   := trap
export OUT_DIR             := .

all: $(OUT_DIR)/$(TARGET).vcd
	$(MAKE) -f ../common.mk script

%.vcd:
	$(MAKE) -f ../common.mk output_dir; \
	$(MAKE) -C ../../third_party/picorv32 test_vcd; \
	cp ../../third_party/picorv32/testbench.vvp $(OUT_DIR)/; \
	cp -R ../../third_party/picorv32/firmware $(OUT_DIR)/; \
	cp ../../third_party/picorv32/testbench.vcd $(OUT_DIR)/

clean:
	$(MAKE) -C ../../third_party/picorv32 $@
	$(MAKE) -f ../common.mk $@
	rm -rf firmware
