module split_tb;
  reg[1:0]  a;
  reg[1:0]  b;
  wire[1:0] o;
  initial begin
    $dumpfile("split.vcd");
    $dumpvars(0, s);
    a = 2'b00;
    b = 2'b01;
    #5;

    a = 2'b11;
    b = 2'b10;
    #5;

    $finish;
  end

  split s(a, b, o);
endmodule
