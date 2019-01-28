Example of Using the tools:
===========================

1. Create the dependency graph in the form of a dotfile
   $ iverilog -t ttb -o depend.dot d_ff.v

1.5 (DEBUG step) Open the dot file graphviz and spot check a couple things
                 to make sure it looks correct

2. Use the parse_dot.py script in the scripts folder to turn the dot file into
   verilog.
   $ python ../scripts/parse_dot.py depend.dot > slice.v

3. Add that verilog to the module you are inspecting
  CAUTION: Nasty bash command ahead. You might want to just do this by hand
  $ cat <(head -n $(($(grep -n "endmodule" d_ff.v | cut -f1 -d:)-1)) d_ff.v) slice.v <(tail -n +$(grep -n "endmodule" d_ff.v | cut -f1 -d:) d_ff.v) > d_ff_signals.v

4. Create the simulator executable
   $ iverilog -t vvp -o d_ff_tb d_ff_tb.v d_ff_signals.v

5. Run the executable
   $ vvp d_ff_tb

6. Run the parse script on the vcd file
   $ python ../scripts/VCD_parse.py d_ff_tb.vcd
