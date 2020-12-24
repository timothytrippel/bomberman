SHELL := /bin/bash

# Directory Structure
BOMBERMAN := ../../bomberman

# Configurations
CLK_BASENAME      := clk
OUT_FILE_BASENAME := $(OUT_DIR)/$(TARGET)

# Check if NUM_TESTS parameter set, default is 1
ifndef NUM_TESTS
	NUM_TESTS := 1
endif

# Check if log parameter set
ifndef LOG
	LOG := 0
endif

# Check program number
ifndef PROGRAM_NUM
	PROGRAM_NUM := 0
endif

all: output_dir bomberman

output_dir:
	@if [ $(OUT_DIR) != "." ]; then \
		echo "Creating output directory ($(OUT_DIR))..."; \
		mkdir -p $(OUT_DIR); \
	fi;

# Bomberman analysis
bomberman: $(OUT_FILE_BASENAME).dot $(OUT_FILE_BASENAME).vcd
	@echo "Analyzing design for counters with time limit: ${TIME_LIMIT} ps ..."; \
	if [ $(LOG) == 0 ]; then \
		time python3 $(BOMBERMAN)/bomberman.py \
			$(START_TIME) \
			$(TIME_LIMIT) \
			$(TIME_RESOLUTION) \
			$(DUT_TOP_MODULE) \
			$(NUM_MALICIOUS_CNTRS) \
			$(D_SIGNAL_BASENAME) \
			$(N_SIGNAL_BASENAME) \
			$^ \
			$(OUT_FILE_BASENAME); \
	else \
		(time python3 $(BOMBERMAN)/bomberman.py \
			$(START_TIME) \
			$(TIME_LIMIT) \
			$(TIME_RESOLUTION) \
			$(DUT_TOP_MODULE) \
			$(NUM_MALICIOUS_CNTRS) \
			$(D_SIGNAL_BASENAME) \
			$(N_SIGNAL_BASENAME) \
			$^ \
			$(OUT_FILE_BASENAME)) &> $(OUT_FILE_BASENAME).$(TIME_LIMIT).ac.log; \
	fi; \
	echo "Done."

# IVL Simulation
$(OUT_FILE_BASENAME).vcd: $(OUT_FILE_BASENAME).vvp
	@echo "Starting simulation with ${NUM_TESTS} tests ..."; \
	if [ $(LOG) == 0 ]; then \
		time vvp $< -DVCD_FILENAME=\"$@\" \
			+num_tests=$(NUM_TESTS) \
			+program_index=${PROGRAM_NUM} \
			+seed_key=${AES_KEY_SEED} \
			+seed_state=${AES_STATE_SEED} \
			+data_seed=${UART_DATA_SEED}; \
	else \
		(time vvp $< -DVCD_FILENAME=\"$@\" \
			+num_tests=$(NUM_TESTS) \
			+program_index=${PROGRAM_NUM} \
			+seed_key=${AES_KEY_SEED} \
			+seed_state=${AES_STATE_SEED} \
			+data_seed=${UART_DATA_SEED}) &> $(OUT_FILE_BASENAME).vcd.log; \
	fi;

# IVL Simulation Binary
$(OUT_FILE_BASENAME).vvp: $(SOURCES) $(TESTBENCH)
	@echo "Generating simulation binary..."; \
	if [ $(LOG) == 0 ]; then \
		iverilog -t vvp -o $@ \
			-DVCD_FILENAME=\"$(OUT_FILE_BASENAME).vcd\" \
			$(INCLUDEDIRS) $^; \
	else \
		(iverilog -t vvp -o $@ \
			-DVCD_FILENAME=\"$(OUT_FILE_BASENAME).vcd\" \
			$(INCLUDEDIRS) $^) &> $(OUT_FILE_BASENAME).vvp.log; \
	fi;

# IVL Target TTB Module Analysis
$(OUT_FILE_BASENAME).dot: $(SOURCES) $(TESTBENCH)
	@echo "Generating DOT graph..."; \
	if [ $(LOG) == 0 ]; then \
		time iverilog -o $@ \
			-pclk=$(CLK_BASENAME) \
			-DVCD_FILENAME=\"$(TARGET).vcd\" \
			-t dfg \
			$(INCLUDEDIRS) $^; \
	else \
		(time iverilog -o $@ \
			-pclk=$(CLK_BASENAME) \
			-DVCD_FILENAME=\"$(TARGET).vcd\" \
			-t dfg \
			$(INCLUDEDIRS) $^) &> $(OUT_FILE_BASENAME).dot.log; \
	fi;

.PHONY: clean output_dir

cleanall: clean
	@$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean:
	@find . -maxdepth 1 -name "*.dot" -print0 | xargs -0 rm -f; \
	find . -maxdepth 1 -name "*.vvp" -print0 | xargs -0 rm -f; \
	find . -maxdepth 1 -name "*.vcd" -print0 | xargs -0 rm -f; \
	find . -maxdepth 1 -name "*.json" -print0 | xargs -0 rm -f; \
	find . -maxdepth 1 -name "*.log" -print0 | xargs -0 rm -f
