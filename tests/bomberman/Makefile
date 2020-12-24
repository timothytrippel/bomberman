# Copyright © 2019, Massachusetts Institute of Technology
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# Directory Structure
TGT_DFG_DIR := ../../tgt-dfg
BOMBERMAN   := ../../bomberman
IVL_DIR     := ../../iverilog/bin

# Configurations
CLK_BASENAME := clk

all: counter d_ff split

# Design Targets
counter:
	$(MAKE) bomberman TGT=counter

d_ff:
	$(MAKE) bomberman TGT=d_ff

split:
	$(MAKE) bomberman TGT=split

# VCD/Dot Analysis Script
bomberman: tgt-dfg $(TGT)/$(TGT).vcd $(TGT)/$(TGT).dot
	python3 $(BOMBERMAN)/bomberman.py 0 -1 1 $(TGT) 0 "none" "none" $(TGT)/$(TGT).dot $(TGT)/$(TGT).vcd $(TGT)/$(TGT).json 

# IVL Simulation (Step 2: VCD Generation)
$(TGT)/$(TGT).vcd: $(TGT)/$(TGT).vvp $(TGT)/$(TGT).dot
	$(IVL_DIR)/vvp $< && mv $(TGT).vcd $@

# IVL Simulation (Step 1: Executable Generation)
$(TGT)/$(TGT).vvp: $(TGT)/$(TGT).v $(TGT)/$(TGT)_tb.v
	$(IVL_DIR)/iverilog -o $@ -t vvp $^

# IVL Target DFG Module Analysis
$(TGT)/$(TGT).dot: $(TGT)/$(TGT).v $(TGT)/$(TGT)_tb.v
	$(IVL_DIR)/iverilog -o $@ -pclk=$(CLK_BASENAME) -t dfg $^
	dot $@ -Tpdf -o $(TGT)/$(TGT).pdf

# Building the IVL Target Module
tgt-dfg: 
	$(MAKE) -C $(TGT_DFG_DIR)

.PHONY: cleanall tgt-dfg counter d_ff split

cleanall:
	@$(MAKE) clean TGT=counter
	@$(MAKE) clean TGT=d_ff
	@$(MAKE) clean TGT=split

clean:
	@rm -f $(TGT)/$(TGT).vcd 
	@rm -f $(TGT)/$(TGT).vvp 
	@rm -f $(TGT)/$(TGT).dot 
	@rm -f $(TGT)/$(TGT).pdf
	@rm -f $(TGT)/$(TGT).*.json

