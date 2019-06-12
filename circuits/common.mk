# Directory Structure
TGT_TTB_DIR :=../../tgt-ttb
SCRIPTS     :=../../scripts

# Configurations
CLK_BASENAME := clk

# Check if NUM_TESTS parameter set, default is 1
ifndef NUM_TESTS
	NUM_TESTS := 1
endif

# Check if log parameter set
ifndef LOG
	LOG := 0
else
	ifndef OUT_DIR
		OUT_DIR := .
	endif
endif

all: output_dir script

output_dir:
	@if [ $(LOG) != 0 ]; then \
		echo "Creating output directory (${OUTPUT_DIR})..."; \
		mkdir -p $(OUT_DIR); \
	fi;

# VCD/Dot Analysis Script
script: $(TARGET).dot $(TARGET).vcd
	@echo "Analyzing design for counters with time limit: ${TIME_LIMIT} ps ..."; \
	if [ $(LOG) == 0 ]; then \
		time pypy $(SCRIPTS)/analyze.py \
			$(START_TIME) \
			$(TIME_LIMIT) \
			$(TIME_RESOLUTION) \
			$(DUT_TOP_MODULE) \
			$(NUM_MALICIOUS_CNTRS) \
			$^ \
			$(TARGET); \
	else \
		(time pypy $(SCRIPTS)/analyze.py \
			$(START_TIME) \
			$(TIME_LIMIT) \
			$(TIME_RESOLUTION) \
			$(DUT_TOP_MODULE) \
			$(NUM_MALICIOUS_CNTRS) \
			$^ \
			$(TARGET)) &> $(OUT_DIR)/$(TARGET).$(TIME_LIMIT).ac.log; \
	fi; \
	echo "Done."

# IVL Simulation (Step 2: VCD Generation)
$(TARGET).vcd: $(TARGET).vvp
	@echo "Starting simulation with ${NUM_TESTS} tests ..."; \
	if [ $(LOG) == 0 ]; then \
		vvp $< -DVCD_FILENAME=\"$@\" \
			+num_tests=$(NUM_TESTS); \
	else \
		vvp $< -DVCD_FILENAME=\"$@\" \
			+num_tests=$(NUM_TESTS) 2>&1 | tee $(OUT_DIR)/$(TARGET).vcd.log; \
	fi;

# IVL Simulation (Step 1: Executable Generation)
$(TARGET).vvp: $(SOURCES) $(TESTBENCH)
	@echo "Generating simulation binary..."; \
	if [ $(LOG) == 0 ]; then \
		iverilog -t vvp -o $@ \
			-DVCD_FILENAME=\"$(TARGET).vcd\" \
			$(INCLUDEDIRS) $^; \
	else \
		(iverilog -t vvp -o $@ \
			-DVCD_FILENAME=\"$(TARGET).vcd\" \
			$(INCLUDEDIRS) $^) &> $(OUT_DIR)/$(TARGET).vvp.log; \
	fi;

# IVL Target TTB Module Analysis
$(TARGET).dot: $(SOURCES) $(TESTBENCH)
	@echo "Generating DOT graph..."; \
	if [ $(LOG) == 0 ]; then \
		time iverilog -o $@ \
			-pclk=$(CLK_BASENAME) \
			-DVCD_FILENAME=\"${TARGET}.vcd\" \
			-t ttb \
			$(INCLUDEDIRS) $^; \
	else \
		(time iverilog -o $@ \
			-pclk=$(CLK_BASENAME) \
			-DVCD_FILENAME=\"${TARGET}.vcd\" \
			-t ttb \
			$(INCLUDEDIRS) $^) &> $(OUT_DIR)/$(TARGET).dot.log; \
	fi;

.PHONY: clean output_dir

cleanall: clean
	@$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean:
	@find . -maxdepth 1 -name "*.dot" -print0 | xargs -0 rm; \
	find . -maxdepth 1 -name "*.vvp" -print0 | xargs -0 rm; \
	find . -maxdepth 1 -name "*.vcd" -print0 | xargs -0 rm; \
	find . -maxdepth 1 -name "*.json" -print0 | xargs -0 rm; \
	find . -maxdepth 1 -name "*.log" -print0 | xargs -0 rm
