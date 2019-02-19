# Directory Structure
TGT_TTB_DIR=../../tgt-ttb
SRC_DIR := verilog
DOT_DIR := dot
PDF_DIR := pdf

# Source/Target Files
SRCS := $(shell find $(SRC_DIR) -type f -name *.v)
DOTS := $(patsubst $(SRC_DIR)/%,$(DOT_DIR)/%,$(SRCS:.v=.dot))

all: tgt-ttb  $(DOTS)

# IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

# TTB Test
$(DOT_DIR)/%.dot: $(SRC_DIR)/%.v
	mkdir dot
	mkdir pdf
	iverilog -o $@ -t ttb $<
	dot $@ -Tpdf -o $(patsubst $(SRC_DIR)/%,$(PDF_DIR)/%,$(<:.v=.pdf))

.PHONY: build cleanall clean

build: tgt-ttb

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)	

clean:
	rm -rf $(DOT_DIR)
	rm -rf $(PDF_DIR)
