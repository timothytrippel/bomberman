ROOT        := $(CURDIR)
TARGET      := or1200

CORES       := $(ROOT)/cores
ORPSOC_RTL  := $(CORES)/top
CLKGEN_RTL  := $(CORES)/clkgen
ARBITER_RTL := $(CORES)/arbiter
DBG_IF_RTL  := $(CORES)/dbg_if
OR1200_RTL  := $(CORES)/or1200
UART_RTL    := $(CORES)/uart16550
RAM_RTL     := $(CORES)/ram_wb
TEST_BENCH  := $(CORES)/bench

SOURCES := $(wildcard $(ORPSOC_RTL)/*.v) $(wildcard $(CLKGEN_RTL)/*.v) $(wildcard $(ARBITER_RTL)/*.v) $(wildcard $(DBG_IF_RTL)/*.v) $(wildcard $(OR1200_RTL)/*.v) $(wildcard $(UART_RTL)/*.v)  $(wildcard $(RAM_RTL)/*.v) 
SOURCES := $(filter-out %-defines.v %-params.v %_defines.v, $(SOURCES))

TESTBENCH := $(wildcard $(TEST_BENCH)/*.v)
TESTBENCH := $(filter-out %-defines.v %-params.v %_defines.v, $(TESTBENCH))

INCLUDEDIRS := -I$(ORPSOC_RTL) -I$(DBG_IF_RTL) -I$(UART_RTL)  -I$(DBG_IF_RTL) -I$(OR1200_RTL) -I$(TEST_BENCH)

# Simulation Confingurations
DUT_TOP_MODULE      := orpsoc_testbench.dut.or1200_top0.or1200_cpu
# DUT_TOP_MODULE      := orpsoc_testbench.dut.or1200_top0
NUM_TESTS           := 2
START_TIME          := 0
TIME_LIMIT          := -1
TIME_RESOLUTION     := 100000
NUM_MALICIOUS_CNTRS := 0

include ../common.mk
