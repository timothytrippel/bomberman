module counter(input wire clk, input wire[63:0] inc, output wire[63:0] count);
  reg [63:0] counter = 64'd0;
  assign count = counter;

  always @(posedge clk) begin
    counter <= #1 counter + inc;
  end
endmodule
