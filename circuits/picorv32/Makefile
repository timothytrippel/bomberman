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

THIRD_PARTY                := ../../third_party
export SOURCES             := $(THIRD_PARTY)/picorv32/picorv32.v
export TESTBENCH           := $(THIRD_PARTY)/picorv32/testbench.v
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
	$(MAKE) -f ../common.mk bomberman

%.vcd:
	$(MAKE) -f ../common.mk output_dir; \
	$(MAKE) -C $(THIRD_PARTY)/picorv32 test_vcd; \
	cp $(THIRD_PARTY)/picorv32/testbench.vvp $(OUT_DIR)/; \
	cp -R $(THIRD_PARTY)/picorv32/firmware $(OUT_DIR)/; \
	cp $(THIRD_PARTY)/picorv32/testbench.vcd $(OUT_DIR)/

clean:
	$(MAKE) -C $(THIRD_PARTY)/picorv32 $@
	$(MAKE) -f ../common.mk $@
	rm -rf firmware
