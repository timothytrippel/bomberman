module counter(input wire rst, input wire clk, input wire [127:0] inc, output wire[127:0] count);
  reg [127:0] counter;
  assign count = counter;

  always @(posedge clk) begin
    if (rst)
      counter <= 128'd0;
    else
      counter <= counter + inc;
  end
endmodule
