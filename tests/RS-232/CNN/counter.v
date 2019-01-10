module counter(input wire clk, input wire[127:0] inc, output wire[127:0] count);
  reg [127:0] counter = 128'd0;
  assign count = counter;

  always @(posedge clk) begin
    counter <= counter + inc;
  end
endmodule
