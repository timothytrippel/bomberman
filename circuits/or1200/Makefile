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

THIRD_PARTY := ../../third_party
TARGET      := or1200
ROOT        := $(THIRD_PARTY)/$(TARGET)
CORES       := $(ROOT)/cores
ORPSOC_RTL  := $(CORES)/top
CLKGEN_RTL  := $(CORES)/clkgen
ARBITER_RTL := $(CORES)/arbiter
DBG_IF_RTL  := $(CORES)/dbg_if
OR1200_RTL  := $(CORES)/or1200
UART_RTL    := $(CORES)/uart16550
RAM_RTL     := $(CORES)/ram_wb
TEST_BENCH  := $(CORES)/bench

SOURCES := \
	$(wildcard $(ORPSOC_RTL)/*.v) \
	$(wildcard $(CLKGEN_RTL)/*.v) \
	$(wildcard $(ARBITER_RTL)/*.v) \
	$(wildcard $(DBG_IF_RTL)/*.v) \
	$(wildcard $(OR1200_RTL)/*.v) \
	$(wildcard $(UART_RTL)/*.v) \
	$(wildcard $(RAM_RTL)/*.v) 
SOURCES := $(filter-out %-defines.v %-params.v %_defines.v, $(SOURCES))

TESTBENCH := $(wildcard $(TEST_BENCH)/*.v)
TESTBENCH := $(filter-out %-defines.v %-params.v %_defines.v, $(TESTBENCH))

INCLUDEDIRS := \
	-I$(ORPSOC_RTL) \
	-I$(DBG_IF_RTL) \
	-I$(UART_RTL)  \
	-I$(DBG_IF_RTL) \
	-I$(OR1200_RTL) \
	-I$(TEST_BENCH)

# Simulation Confingurations
DUT_TOP_MODULE      := orpsoc_testbench.dut.or1200_top0.or1200_cpu
NUM_TESTS           := 2
START_TIME          := 0
TIME_LIMIT          := -1
TIME_RESOLUTION     := 10000
NUM_MALICIOUS_CNTRS := 0
D_SIGNAL_BASENAME   := clk
N_SIGNAL_BASENAME   := wb_pc
OUT_DIR             := .

include ../common.mk
