`include "uart_defines.v"

module counter(input wire clk, output wire[`CNT_WIDTH - 1:0] count);
  reg [`CNT_WIDTH - 1:0] counter = `CNT_WIDTH'd0;
  assign count = counter;

  always @(posedge clk) begin
    counter <= #1 counter + `CNT_WIDTH'h1;
  end
endmodule
