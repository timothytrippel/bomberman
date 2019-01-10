SCRIPTS = ../../scripts

TARGET = split

SOURCES   = $(TARGET).v
TESTBENCH = $(TARGET)_tb.v


all: script

script: $(TARGET).vcd
	python $(SCRIPTS)/analyze.py $(TARGET).dot $(TARGET).vcd

$(TARGET).vcd: $(TARGET).vvp $(TARGET).dot
	vvp $<

$(TARGET).dot: $(SOURCES) $(TESTBENCH)
	iverilog -o $@ -t ttb $^

$(TARGET).vvp: $(SOURCES) $(TESTBENCH)
	iverilog -o $@ -t vvp $^

.PHONY: clean

clean:
	$(RM) $(TARGET).vvp $(TARGET).dot $(TARGET).vcd
