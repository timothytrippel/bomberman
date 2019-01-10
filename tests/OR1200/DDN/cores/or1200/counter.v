module counter(input wire rst, input wire clk, output wire[127:0] count);
  reg [127:0] counter = 128'd0;
  assign count = counter;

  always @(posedge clk) begin
    if (rst)
      counter <= #1 128'd0;
    else
      counter <= #1 counter + 128'h1;
  end
endmodule
