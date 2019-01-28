module counter(input wire clk, input wire[127:0] state, output wire[63:0] count);
  reg [63:0] counter = 128'd9;
  assign count = counter;

  always @(posedge clk) begin
    counter <= #1 counter + state[63:0];
  end
endmodule
