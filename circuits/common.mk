SCRIPTS   = ../../../scripts

all: script

script: $(TARGET).dot $(TARGET).vcd
	time python $(SCRIPTS)/analyze.py $^

$(TARGET).vcd: $(TARGET).vvp $(TARGET).dot
	vvp $<

$(TARGET).dot: $(SOURCES) $(TESTBENCH)
	time iverilog -t ttb -o $@ $(INCLUDEDIRS) $^

$(TARGET).vvp: $(SOURCES) $(TESTBENCH)
	iverilog -t vvp -o $@ $(INCLUDEDIRS) $^

.PHONY: clean

clean:
	$(RM) $(TARGET).vvp $(TARGET).dot $(TARGET).vcd
