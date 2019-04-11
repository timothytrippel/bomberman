# Directory Structure
TGT_TTB_DIR=../../../tgt-ttb
SCRIPTS := ../../../scripts

# Target Design (top module)
TARGET := d_ff

# Source/Testbench Files
SOURCES   := $(TARGET).v
TESTBENCH := $(TARGET)_tb.v

# Configurations
CLK_BASENAME := clk

all: tgt-ttb script

# Building the IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

# VCD Analysis Script
script: $(TARGET).vcd
	python2 $(SCRIPTS)/analyze.py $(TARGET).dot $(TARGET).vcd

# IVL Simulation (Step 2: VCD Generation)
$(TARGET).vcd: $(TARGET).vvp $(TARGET).dot
	vvp $<

# IVL Simulation (Step 2: Executable Generation)
$(TARGET).vvp: $(SOURCES) $(TESTBENCH)
	iverilog -o $@ -t vvp $^

# IVL Target TTB Module Analysis
$(TARGET).dot: $(SOURCES) $(TESTBENCH)
	iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $^
	dot $@ -Tpdf -o $(TARGET).pdf

.PHONY: clean

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)	

clean:
	$(RM) $(TARGET).vvp $(TARGET).dot $(TARGET).pdf $(TARGET).vcd
