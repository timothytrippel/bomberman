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

THIRD_PARTY         := ../../third_party
TARGET              := uart
SOURCES             := $(wildcard $(THIRD_PARTY)/uart16550/*.v)
TESTBENCH           := uart_top_tb.v $(wildcard $(THIRD_PARTY)/misc/*.v)
INCLUDEDIRS         := -I$(THIRD_PARTY)/uart16550 -I$(THIRD_PARTY)/misc
DUT_TOP_MODULE      := uart_top_tb.uart1
NUM_TESTS           ?= 1
START_TIME          ?= 0
TIME_LIMIT          ?= -1
TIME_RESOLUTION     ?= 1000
NUM_MALICIOUS_CNTRS ?= 0
D_SIGNAL_BASENAME   ?= wb_clk_i
N_SIGNAL_BASENAME   ?= rf_count
OUT_DIR             ?= .
UART_DATA_SEED      ?= 0

include ../common.mk
