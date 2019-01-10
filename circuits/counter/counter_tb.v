module counter_tb;
  reg clk;
  reg rst;

  wire[3:0] count;

  counter c(clk, rst, count);

  always 
    #5 clk = ~clk;

  initial begin
    $dumpfile("counter.vcd");
    $dumpvars(0, c);
    clk = 0;
    rst = 1;

    #11;
    rst = 0;

    #200;
    $finish;
  end
endmodule

