module counter(input wire clk, input wire evnt, output wire[127:0] count);
  reg [127:0] counter = 128'd0;
  assign count = counter;

  always @(posedge clk) begin
    if (evnt)
      counter <= ounter + 128'h1;
    else
      counter <= counter;
  end
endmodule
