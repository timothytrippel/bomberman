# Directory Structure
TGT_TTB_DIR=../../tgt-ttb
SRC_DIR      := verilog
DOT_DIR      := dot
GOLD_DOT_DIR := golden_dot
PDF_DIR      := pdf
IGNORES_DIR  := signal_ignores

# Source/Target Files
SRCS      := $(shell find $(SRC_DIR) -type f -name *.v)
DOTS      := $(patsubst $(SRC_DIR)/%,$(DOT_DIR)/%,$(SRCS:.v=.dot))
GOLD_DOTS := $(patsubst $(SRC_DIR)/%,$(GOLD_DOT_DIR)/%,$(SRCS:.v=.dot))
TESTS     := $(addsuffix .test, $(basename $(SRCS)))

# Colors
NO_COLOR="\x1b[0m"
GREEN_COLOR="\x1b[32;01m"
RED_COLOR="\x1b[31;01m"
YELLOW_COLOR="\x1b[33;01m"

# Message Strings
PASS_STRING:="$(GREEN_COLOR)[PASS]$(NO_COLOR)"
FAIL_STRING:="$(RED_COLOR)[FAIL]$(NO_COLOR)"
ERROR_STRING:="$(RED_COLOR)[ERROR]$(NO_COLOR)"
WARN_STRING:="$(YELLOW_COLOR)[WARNING]$(NO_COLOR)"

# Configurations
CLK_BASENAME := clk

# For debuging type: make print-<VARIABLE>
print-%  : ; @echo $* = $($*)

all: tgt-ttb $(DOTS)

# Run All Tests
test: tgt-ttb $(DOTS) $(TESTS)

$(SRC_DIR)/%.test: $(DOT_DIR)/%.dot
	@if diff -q $< $(patsubst $(DOT_DIR)/%,$(GOLD_DOT_DIR)/%,$<) > /dev/null; then \
		echo "Test $(PASS_STRING): $(basename $@)"; \
	else \
		echo "Test $(FAIL_STRING): $(basename $@)"; \
	fi;

# IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

# IVL Target TTB Module Analysis
$(DOT_DIR)/%.dot: $(SRC_DIR)/%.v
	mkdir -p dot
	mkdir -p pdf
	iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $<
# 	iverilog -o $@ -pclk=$(CLK_BASENAME) -pignore_filepath=$(patsubst $(SRC_DIR)/%,$(IGNORES_DIR)/%,$(<:.v=.txt)) -t ttb $<
	dot $@ -Tpdf -o $(patsubst $(SRC_DIR)/%,$(PDF_DIR)/%,$(<:.v=.pdf))

.PHONY: build cleanall clean

build: tgt-ttb

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)	

clean:
	rm -rf $(DOT_DIR)
	rm -rf $(PDF_DIR)
