# Directory Structure
TGT_TTB_DIR :=../../tgt-ttb
SCRIPTS     :=../../scripts
IVL_DIR     := ../../iverilog/bin

# Configurations
CLK_BASENAME := clk

# Check if NUM_TESTS parameter set, default is 1
ifndef NUM_TESTS
	NUM_TESTS := 1
endif

# Check if log parameter set
ifndef LOG
	LOG := 0
endif

# Check output directory
ifndef OUT_DIR
	OUT_DIR := .
	OUT_FILE_BASENAME := $(TARGET)
else
	OUT_FILE_BASENAME := $(OUT_DIR)/$(TARGET)
endif

# Check program number
ifndef PROGRAM_NUM
	PROGRAM_NUM := 0
endif

all: output_dir script

output_dir:
	@if [ $(OUT_DIR) != "." ]; then \
		echo "Creating output directory ($(OUT_DIR))..."; \
		mkdir -p $(OUT_DIR); \
	fi;

# VCD/Dot Analysis Script
script: $(OUT_FILE_BASENAME).dot $(OUT_FILE_BASENAME).vcd
	@echo "Analyzing design for counters with time limit: ${TIME_LIMIT} ps ..."; \
	if [ $(LOG) == 0 ]; then \
		time python2 $(SCRIPTS)/analyze.py \
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
		(time python2 $(SCRIPTS)/analyze.py \
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

# IVL Simulation (Step 2: VCD Generation)
$(OUT_FILE_BASENAME).vcd: $(OUT_FILE_BASENAME).vvp
	@echo "Starting simulation with ${NUM_TESTS} tests ..."; \
	if [ $(LOG) == 0 ]; then \
		vvp $< -DVCD_FILENAME=\"$@\" \
			+num_tests=$(NUM_TESTS) \
			+program_index=${PROGRAM_NUM} \
			+seed_key=${AES_KEY_SEED} \
			+seed_state=${AES_STATE_SEED} \
			+data_seed=${UART_DATA_SEED}; \
	else \
		vvp $< -DVCD_FILENAME=\"$@\" \
			+num_tests=$(NUM_TESTS) \
			+program_index=${PROGRAM_NUM} \
			+seed_key=${AES_KEY_SEED} \
			+seed_state=${AES_STATE_SEED} \
			+data_seed=${UART_DATA_SEED} 2>&1 | tee $(OUT_FILE_BASENAME).vcd.log; \
	fi;

# IVL Simulation (Step 1: Executable Generation)
$(OUT_FILE_BASENAME).vvp: $(SOURCES) $(TESTBENCH)
	@echo "Generating simulation binary..."; \
	if [ $(LOG) == 0 ]; then \
		$(IVL_DIR)/iverilog -t vvp -o $@ \
			-DVCD_FILENAME=\"$(OUT_FILE_BASENAME).vcd\" \
			$(INCLUDEDIRS) $^; \
	else \
		($(IVL_DIR)/iverilog -t vvp -o $@ \
			-DVCD_FILENAME=\"$(OUT_FILE_BASENAME).vcd\" \
			$(INCLUDEDIRS) $^) &> $(OUT_FILE_BASENAME).vvp.log; \
	fi;

# IVL Target TTB Module Analysis
$(OUT_FILE_BASENAME).dot: $(SOURCES) $(TESTBENCH)
	@echo "Generating DOT graph..."; \
	if [ $(LOG) == 0 ]; then \
		time $(IVL_DIR)/iverilog -o $@ \
			-pclk=$(CLK_BASENAME) \
			-DVCD_FILENAME=\"$(TARGET).vcd\" \
			-t ttb \
			$(INCLUDEDIRS) $^; \
	else \
		(time $(IVL_DIR)/iverilog -o $@ \
			-pclk=$(CLK_BASENAME) \
			-DVCD_FILENAME=\"$(TARGET).vcd\" \
			-t ttb \
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
