module counter(input wire clk, input wire[127:0] state, output wire[127:0] count);
  reg [127:0] counter = 128'd9;
  assign count = counter;

  always @(posedge clk) begin
    counter <= counter + state;
  end
endmodule
