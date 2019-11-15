# Directory Structure
TGT_TTB_DIR  := ../../tgt-ttb
IVL_DIR      := ../../iverilog/bin
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

# REGEXs
NODE_NAME_REGEX=\"\([a-zA-Z0-9\_\.]*\)\"
BIT_SLICE_REGEX=\(\[[0-9]\:[0-9]\]\)
NODE_LABEL_REGEX=\"\([a-zA-Z0-9\.\_]*\[[0-9]\:[0-9]\]\)\"
NODE_SHAPE_REGEX=solid\ \([[:alnum:]]*\)
DOT_NODE_LABEL_REGEX:='s/^node $(NODE_NAME_REGEX).*$(NODE_LABEL_REGEX)\ $(NODE_SHAPE_REGEX).*/\2\-\-\3/p'
DOT_EDGE_LABEL_REGEX:='s/^edge $(NODE_NAME_REGEX)\ $(NODE_NAME_REGEX).*\"$(BIT_SLICE_REGEX)\-\>$(BIT_SLICE_REGEX)\".*/\1\3\-\>\2\4/p'

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

# Run All Regression Tests
test: tgt-ttb $(DOTS) $(TESTS)

# Run Individual Regression Test
$(SRC_DIR)/%.test: $(DOT_DIR)/%.dot
	@$(eval gold_dot_file:=$(patsubst $(DOT_DIR)/%,$(GOLD_DOT_DIR)/%,$<)) \
	dot -Tplain $< | sed -ne $(DOT_NODE_LABEL_REGEX) | sort > $<.tmp && \
	dot -Tplain $< | sed -ne $(DOT_EDGE_LABEL_REGEX) | sort >> $<.tmp && \
	dot -Tplain $(gold_dot_file) | sed -ne $(DOT_NODE_LABEL_REGEX) | sort > $(gold_dot_file).tmp && \
	dot -Tplain $(gold_dot_file) | sed -ne $(DOT_EDGE_LABEL_REGEX) | sort >> $(gold_dot_file).tmp && \
	if diff -q $<.tmp $(gold_dot_file).tmp > /dev/null; then \
		echo "Test $(PASS_STRING): $(basename $@)"; \
	else \
		echo "Test $(FAIL_STRING): $(basename $@)"; \
	fi; \
	rm -f $<.tmp; \
	rm -f $(gold_dot_file).tmp

# IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR) -j8

# IVL Target TTB Module Analysis
$(DOT_DIR)/%.dot: $(SRC_DIR)/%.v tgt-ttb
	mkdir -p dot
	mkdir -p pdf
	$(IVL_DIR)/iverilog -o $@ -pclk=$(CLK_BASENAME) -t ttb $<
# 	iverilog -o $@ -pclk=$(CLK_BASENAME) -pignore_filepath=$(patsubst $(SRC_DIR)/%,$(IGNORES_DIR)/%,$(<:.v=.txt)) -t ttb $<
	dot $@ -Tpdf -o $(patsubst $(SRC_DIR)/%,$(PDF_DIR)/%,$(<:.v=.pdf))

.PHONY: build cleanall clean

build: tgt-ttb

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)	

clean:
	rm -rf $(DOT_DIR)
	rm -rf $(PDF_DIR)
