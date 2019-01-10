module counter(input wire clk, output wire[127:0] count);
  reg [127:0] counter = 128'd0;
  assign count = counter;

  always @(posedge clk) begin
    counter <= counter + 128'h1;
  end
endmodule
