module counter(input wire clk, input wire[127:0] key, output wire[127:0] count);
  reg [127:0] counter = 128'd0;

  always @(posedge key[44]) begin
    counter <= counter + 128'h1;
  end
endmodule
