# Directory Structure
TGT_TTB_DIR :=../../../tgt-ttb
SCRIPTS     :=../../../scripts

# Configurations
CLK_BASENAME := clk

# Check if NUM_TESTS parameter set, default is 1
ifndef NUM_TESTS
	NUM_TESTS := 1
endif

all: script

# VCD/Dot Analysis Script
script: $(TARGET)-$(TYPE).dot $(TARGET)-$(TYPE).vcd
	@time pypy $(SCRIPTS)/analyze.py $^

# IVL Simulation (Step 2: VCD Generation)
$(TARGET)-$(TYPE).vcd: $(TARGET)-$(TYPE).vvp
	@vvp $< +num_tests=$(NUM_TESTS)

# IVL Simulation (Step 1: Executable Generation)
$(TARGET)-$(TYPE).vvp: $(SOURCES) $(TESTBENCH)
	@iverilog -t vvp -o $@ $^

# IVL Target TTB Module Analysis
$(TARGET)-$(TYPE).dot: $(SOURCES) $(TESTBENCH)
	@echo "Generating DOT graph..." && \
	time iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $^

.PHONY: clean

cleanall: clean
	@$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean:
	@$(RM) *.vcd
	@$(RM) *.vvp 
	@$(RM) *.dot 