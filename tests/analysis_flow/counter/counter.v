module counter(input wire clk, input wire rst, output wire[3:0] count);
  reg [3:0] counter;
  reg some_value;
  assign count = counter;

  always @(posedge clk) begin
    if (rst) begin
      counter <= #1 0;
      some_value <= #1 1'b0;
    end else begin
      counter <= #1 counter + 4'h1;
    end
  end
endmodule
