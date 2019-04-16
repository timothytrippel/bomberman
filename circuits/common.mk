# Directory Structure
TGT_TTB_DIR :=../../../tgt-ttb
SCRIPTS     :=../../../scripts

# Configurations
CLK_BASENAME := clk

all: script

# VCD/Dot Analysis Script
script: tgt-ttb $(TARGET).dot $(TARGET).vcd
	@time python $(SCRIPTS)/analyze.py $^

# IVL Simulation (Step 2: VCD Generation)
$(TARGET).vcd: $(TARGET).vvp $(TARGET).dot
	@vvp $<

# IVL Simulation (Step 1: Executable Generation)
$(TARGET).vvp: $(SOURCES) $(TESTBENCH)
	@iverilog -t vvp -o $@ $(INCLUDEDIRS) $^

# IVL Target TTB Module Analysis
$(TARGET).dot: $(SOURCES) $(TESTBENCH)
	@echo "Generating DOT graph..." && \
	time iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $(INCLUDEDIRS) $^

# Building the IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

.PHONY: clean tgt-ttb

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean:
	$(RM) $(TARGET).vvp $(TARGET).dot $(TARGET).vcd
