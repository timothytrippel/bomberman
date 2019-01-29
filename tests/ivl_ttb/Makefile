TGT_TTB_DIR=../../tgt-ttb

SRCS=$(wildcard *.v)
DOTS=$(SRCS:.v=.dot)

all: tgt-ttb  $(DOTS)

# IVL Target Module
tgt-ttb: 
	$(MAKE) -C $(TGT_TTB_DIR)

# TTB Test
%.dot: %.v
	iverilog -o $@ -t ttb $<
	dot $@ -Tpdf -o $(<:.v=.pdf)

.PHONY: build cleanall clean

build: tgt-ttb

cleanall: clean
	$(MAKE) cleanall -C $(TGT_TTB_DIR)	

clean:
	rm -f *.dot
