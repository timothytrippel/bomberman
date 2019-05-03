# Directory Structure
TGT_TTB_DIR :=../../tgt-ttb
SCRIPTS     := ../../scripts

# Configurations
CLK_BASENAME := clk

all: counter d_ff split

# Design Targets
counter:
	$(MAKE) script TGT=counter

d_ff:
	$(MAKE) script TGT=d_ff

split:
	$(MAKE) script TGT=split

# VCD/Dot Analysis Script
script: tgt-ttb $(TGT)/$(TGT).vcd $(TGT)/$(TGT).dot
	python2 $(SCRIPTS)/analyze.py $(TGT)/$(TGT).dot $(TGT)/$(TGT).vcd

# IVL Simulation (Step 2: VCD Generation)
$(TGT)/$(TGT).vcd: $(TGT)/$(TGT).vvp $(TGT)/$(TGT).dot
	vvp $< && mv $(TGT).vcd $@

# IVL Simulation (Step 1: Executable Generation)
$(TGT)/$(TGT).vvp: $(TGT)/$(TGT).v $(TGT)/$(TGT)_tb.v
	iverilog -o $@ -t vvp $^

# IVL Target TTB Module Analysis
$(TGT)/$(TGT).dot: $(TGT)/$(TGT).v $(TGT)/$(TGT)_tb.v
	iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $^
	dot $@ -Tpdf -o $(TGT)/$(TGT).pdf

# Building the IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

.PHONY: clean tgt-ttb counter d_ff split

cleanall:
	@$(MAKE) clean TGT=counter
	@$(MAKE) clean TGT=d_ff
	@$(MAKE) clean TGT=split
# 	$(MAKE) cleanall -C $(TGT_TTB_DIR)

clean:
	@rm -f $(TGT)/$(TGT).vcd $(TGT)/$(TGT).vvp $(TGT)/$(TGT).dot $(TGT)/$(TGT).pdf
