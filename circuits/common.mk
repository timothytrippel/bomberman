# Directory Structure
TGT_TTB_DIR :=../../../tgt-ttb
SCRIPTS     :=../../../scripts

# Configurations
CLK_BASENAME := clk

all: script

# VCD/Dot Analysis Script
script: $(TARGET)-$(TYPE).dot $(TARGET)-$(TYPE).vcd
	@time pypy $(SCRIPTS)/analyze.py $^

# IVL Simulation (Step 2: VCD Generation)
$(TARGET)-$(TYPE).vcd: $(TARGET)-$(TYPE).vvp
	@vvp $<

# IVL Simulation (Step 1: Executable Generation)
$(TARGET)-$(TYPE).vvp: $(SOURCES) $(TESTBENCH)
	@iverilog -t vvp -o $@ $(INCLUDEDIRS) $^

# IVL Target TTB Module Analysis
$(TARGET)-$(TYPE).dot: $(SOURCES) $(TESTBENCH)
	@echo "Generating DOT graph..." && \
	time iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $(INCLUDEDIRS) $^

.PHONY: clean

cleanall: clean
	@$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean:
	@$(RM) $(TARGET)-$(TYPE).vcd
	@$(RM) $(TARGET)-$(TYPE).vvp 
	@$(RM) $(TARGET)-$(TYPE).dot 