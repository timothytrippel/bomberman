module counter(input wire clk, input wire en, input wire[7:0] inc, output wire[7:0] count);
  reg [7:0] counter = 8'd0;
  assign count = counter;

  always @(posedge clk) begin
    if (en) begin
      counter <= #1 counter + inc;
    end else begin
      counter <= #1 counter;
    end
  end
endmodule
